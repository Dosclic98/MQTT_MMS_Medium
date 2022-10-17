#include "EvilServer.h"

//#include "inet/applications/common/SocketTag_m.h"
#include "inet/common/socket/SocketTag_m.h"
#include "../../message/mms/MmsMessage_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/packet/Message.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/TimeTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/tcp/TcpCommand_m.h"

namespace inet {

Define_Module(EvilServer);

void EvilServer::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        //statistics
        msgsRcvd = msgsSent = bytesRcvd = bytesSent = 0;

        WATCH(msgsRcvd);
        WATCH(msgsSent);
        WATCH(bytesRcvd);
        WATCH(bytesSent);
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        const char *localAddress = par("localAddress");
        int localPort = par("localPort");
        socket.setOutputGate(gate("socketOut"));
        socket.bind(localAddress[0] ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
        socket.listen();
        departureEvent = new cMessage("Server Departure");
        evilServerStatus = false;
        serverConnId = -999;

        measureBlockSignal = registerSignal("measureBlockSignal");
        measureCompromisedSignal = registerSignal("measureCompromisedSignal");
        genericRequestBlockSignal = registerSignal("genericRequestBlockSignal");
        genericRequestCompromisedSignal = registerSignal("genericRequestCompromisedSignal");
        genericResponseBlockSignal = registerSignal("genericResponseBlockSignal");
        genericResponseCompromisedSignal = registerSignal("genericResponseCompromisedSignal");
        cModule *node = findContainingNode(this);
        NodeStatus *nodeStatus = node ? check_and_cast_nullable<NodeStatus *>(node->getSubmodule("status")) : nullptr;
        bool isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError("This module doesn't support starting in node DOWN state");
    }
}

void EvilServer::sendOrSchedule(cMessage *msg, simtime_t delay)
{
    if (delay == 0)
        sendBack(msg);
    else
        scheduleAt(simTime() + delay, msg);
}

void EvilServer::sendBack(cMessage *msg)
{
    Packet *packet = dynamic_cast<Packet *>(msg);

    if (packet) {
        msgsSent++;
        bytesSent += packet->getByteLength();
        emit(packetSentSignal, packet);

        EV_INFO << "sending \"" << packet->getName() << "\" to TCP, " << packet->getByteLength() << " bytes\n";
    }
    else {
        EV_INFO << "sending \"" << msg->getName() << "\" to TCP\n";
    }

    auto& tags = check_and_cast<ITaggedObject *>(msg)->getTags();
    tags.addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::tcp);
    send(msg, "socketOut");
}

void EvilServer::sendPacketDeparture(int connId, B requestedBytes, B replyLength, int messageKind, int clientConnId) {
    double p = this->uniform(0.0, 1.0);
    if (messageKind == 1) {
        if (p < 0.15) { //Block
            emit(measureBlockSignal, true);
            return;
        } else if (p < 0.4){ //Compromise
            emit(measureCompromisedSignal, true);
        }
    } else if(messageKind == 2) {
        if (p < 0.2) { // Block
            emit(genericRequestBlockSignal, true);
            return;
        } else if (p < 0.8) { // Compromise
            emit(genericRequestCompromisedSignal, true);
        }
    } else if (messageKind == 3) {
        if (p < 0.1) { // Block
            emit(genericResponseBlockSignal, true);
            return;
        } else if (p < 0.6) { // Compromise
            emit(genericResponseCompromisedSignal, true);
        }
    }
    Packet *outPacket = new Packet("Generic Data", TCP_C_SEND);
    outPacket->addTag<SocketReq>()->setSocketId(connId);
    const auto& payload = makeShared<MmsMessage>();
    payload->setMessageKind(messageKind);
    payload->setChunkLength(requestedBytes);
    payload->setExpectedReplyLength(replyLength);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    payload->setConnId(clientConnId);
    payload->setServerClose(false);
    outPacket->insertAtBack(payload);
    sendOrSchedule(outPacket, SimTime(round(par("replyDelay").doubleValue()), SIMTIME_MS));
}

void EvilServer::handleDeparture()
{
    Packet *packet = check_and_cast<Packet *>(serverQueue.pop());
    int connId = packet->getTag<SocketInd>()->getSocketId();
    ChunkQueue &queue = socketQueue[connId];
    auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
    queue.push(chunk);
    emit(packetReceivedSignal, packet);
    bool doClose = false;
    while (const auto& appmsg = queue.pop<MmsMessage>(b(-1), Chunk::PF_ALLOW_NULLPTR)) {
        msgsRcvd++;
        bytesRcvd += B(appmsg->getChunkLength()).get();
        B requestedBytes = appmsg->getExpectedReplyLength();
        if(appmsg->getMessageKind() == 0) { // Register listener
            if(serverConnId != -999) sendPacketDeparture(serverConnId, B(100), B(100), 0, connId);
            else {
                MmsMessage msg;
                msg.setMessageKind(appmsg->getMessageKind());
                msg.setConnId(connId);
                msg.setExpectedReplyLength(appmsg->getExpectedReplyLength());
                msg.setChunkLength(appmsg->getChunkLength());
                delayedPkts.push_back(msg);
            }
        }
        else if(appmsg->getMessageKind() == 1) sendPacketDeparture(appmsg->getConnId(), requestedBytes, B(0), 1, -1);
        else if (appmsg->getMessageKind() == 2){ // Generic Request From Client
            if(serverConnId != -999) sendPacketDeparture(serverConnId, B(100), B(100), 2, connId);
            else {
                MmsMessage msg;
                msg.setMessageKind(appmsg->getMessageKind());
                msg.setConnId(connId);
                msg.setExpectedReplyLength(appmsg->getExpectedReplyLength());
                msg.setChunkLength(appmsg->getChunkLength());
                delayedPkts.push_back(msg);
            }
        }
        else if (appmsg->getMessageKind() == 3) { //Generic Response From Server
            if (requestedBytes > B(0)) sendPacketDeparture(appmsg->getConnId(), B(100), B(100), 3, -1);
        }
        else if(appmsg->getMessageKind() == 99) { //Server ID
            serverConnId = connId;
            for(auto msg : delayedPkts) { // Send all Delayed Packets
                sendPacketDeparture(serverConnId, B(100), B(100), msg.getMessageKind(), msg.getConnId());
            }
            delayedPkts.clear();
        }
        else { /* Bad Request, not present in MITM */}
        if (appmsg->getServerClose()) {
            doClose = true;
            break;
        }
    }
    delete packet;

    if (doClose) {
    	EV << "CLOOOOOSE";
        auto request = new Request("close", TCP_C_CLOSE);
        TcpCommand *cmd = new TcpCommand();
        request->addTag<SocketReq>()->setSocketId(connId);
        request->setControlInfo(cmd);
        sendOrSchedule(request, SimTime(round(par("replyDelay").doubleValue()), SIMTIME_MS));
    }

    if(serverQueue.getLength() > 0) {
        scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS),
                departureEvent);
    } else evilServerStatus = false;
}

void EvilServer::handleMessage(cMessage *msg)
{
    if (msg == departureEvent) handleDeparture();
    else if (msg->isSelfMessage()) {
        sendBack(msg);
    }
    else if (msg->getKind() == TCP_I_PEER_CLOSED) {

        // we'll close too, but only after there's surely no message
        // pending to be sent back in this connection
        int connId = check_and_cast<Indication *>(msg)->getTag<SocketInd>()->getSocketId();
        delete msg;
        auto request = new Request("close", TCP_C_CLOSE);
        request->addTag<SocketReq>()->setSocketId(connId);
        sendOrSchedule(request, SimTime(round(par("replyDelay").doubleValue()), SIMTIME_MS));
    }
    else if (msg->getKind() == TCP_I_DATA || msg->getKind() == TCP_I_URGENT_DATA) {
        if(!evilServerStatus) {
            evilServerStatus = true;
            serverQueue.insert(msg);
            scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS), departureEvent);
        }
        else serverQueue.insert(msg);
    }
    else if (msg->getKind() == TCP_I_AVAILABLE)
        socket.processMessage(msg);
    else {
        // some indication -- ignore
        EV_WARN << "drop msg: " << msg->getName() << ", kind:" << msg->getKind() << "("
                << cEnum::get("inet::TcpStatusInd")->getStringFor(msg->getKind()) << ")\n";
        delete msg;
    }
}

void EvilServer::refreshDisplay() const
{
    char buf[64];
    sprintf(buf, "rcvd: %ld pks %ld bytes\nsent: %ld pks %ld bytes", msgsRcvd, bytesRcvd, msgsSent, bytesSent);
    getDisplayString().setTagArg("t", 0, buf);
}

void EvilServer::finish()
{
    cancelAndDelete(departureEvent);
    serverQueue.clear();
    delayedPkts.clear();
    EV_INFO << getFullPath() << ": sent " << bytesSent << " bytes in " << msgsSent << " packets\n";
    EV_INFO << getFullPath() << ": received " << bytesRcvd << " bytes in " << msgsRcvd << " packets\n";
}

} // namespace inet
