#include "MmsServer.h"

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

Define_Module(MmsServer);

void MmsServer::initialize(int stage)
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
        sendDataEvent = new cMessage("Register Data To Send Event");
        serverStatus = false;
        scheduleAt(1, sendDataEvent);

        cModule *node = findContainingNode(this);
        NodeStatus *nodeStatus = node ? check_and_cast_nullable<NodeStatus *>(node->getSubmodule("status")) : nullptr;
        bool isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError("This module doesn't support starting in node DOWN state");
    }
}

void MmsServer::sendOrSchedule(cMessage *msg, simtime_t delay)
{
    if (delay == 0)
        sendBack(msg);
    else
        scheduleAt(simTime() + delay, msg);
}

void MmsServer::sendBack(cMessage *msg)
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

void MmsServer::sendPacketDeparture(int connId, B requestedBytes, B replyLength, int messageKind) {
    Packet *outPacket = new Packet("Generic Data", TCP_C_SEND);
    outPacket->addTag<SocketReq>()->setSocketId(connId);
    const auto& payload = makeShared<MmsMessage>();
    payload->setMessageKind(messageKind);
    payload->setChunkLength(requestedBytes);
    payload->setExpectedReplyLength(replyLength);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    outPacket->insertAtBack(payload);
    sendOrSchedule(outPacket, SimTime(par("replyDelay").intValue(), SIMTIME_MS));
}

void MmsServer::handleDeparture()
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
        if(appmsg->getMessageKind() == 0) { // Register a listener
            clientConnIdList.push_back(connId);
        }
        else if(appmsg->getMessageKind() == 1) { // Send data to listeners
            for (auto const& listenerConnId : clientConnIdList) {
                if (requestedBytes > B(0)) {
                    sendPacketDeparture(listenerConnId, B(100), B(0), 1);
                }
            }
        }
        else if (appmsg->getMessageKind() == 2) { // Response to Generic Request
            if (requestedBytes > B(0)) {
                sendPacketDeparture(connId, requestedBytes, B(0), 3);
            }
        }
        else {
            //Bad Request
        }
        if (appmsg->getServerClose()) {
            doClose = true;
            break;
        }
    }
    delete packet;

    if (doClose) {
        auto request = new Request("close", TCP_C_CLOSE);
        TcpCommand *cmd = new TcpCommand();
        request->addTag<SocketReq>()->setSocketId(connId);
        request->setControlInfo(cmd);
        clientConnIdList.remove(connId);
        sendOrSchedule(request, SimTime(par("replyDelay").intValue(), SIMTIME_MS));
    }

    if(serverQueue.getLength() > 0) {
        scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS),
                departureEvent);
    } else serverStatus = false;
}

void MmsServer::handleMessage(cMessage *msg)
{
    if (msg == departureEvent) handleDeparture();

    else if(msg == sendDataEvent) {
        auto pkt = new Packet("SendData");
        pkt->addTag<SocketInd>()->setSocketId(-1);
        const auto& payload = makeShared<MmsMessage>();
        payload->setMessageKind(1);
        payload->setChunkLength(B(100));
        payload->setExpectedReplyLength(B(100));
        payload->setServerClose(false);
        pkt->insertAtBack(payload);
        if (!clientConnIdList.empty()) {
            if(!serverStatus) {
                serverStatus = true;
                serverQueue.insert(pkt);
                scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS),
                        departureEvent);
            }
            else serverQueue.insert(pkt);
        }
        scheduleAt(simTime() + SimTime(par("emitInterval").intValue(), SIMTIME_MS), sendDataEvent);
    }

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
        clientConnIdList.remove(connId);
        sendOrSchedule(request, SimTime(par("replyDelay").intValue(), SIMTIME_MS));
    }
    else if (msg->getKind() == TCP_I_DATA || msg->getKind() == TCP_I_URGENT_DATA) {

        if(!serverStatus) {
            serverStatus = true;
            serverQueue.insert(msg);
            scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS),
                    departureEvent);
        }
        else serverQueue.insert(msg);
    }
    else if (msg->getKind() == TCP_I_AVAILABLE)
        socket.processMessage(msg);
    else {
        // some indication -- ignore
        EV_WARN << "drop msg: " << msg->getName() << ", kind:" << msg->getKind() << "(" << cEnum::get("inet::TcpStatusInd")->getStringFor(msg->getKind()) << ")\n";
        delete msg;
    }
}

void MmsServer::refreshDisplay() const
{
    char buf[64];
    sprintf(buf, "rcvd: %ld pks %ld bytes\nsent: %ld pks %ld bytes", msgsRcvd, bytesRcvd, msgsSent, bytesSent);
    getDisplayString().setTagArg("t", 0, buf);
}

void MmsServer::finish()
{
    cancelAndDelete(departureEvent);
    cancelAndDelete(sendDataEvent);
    serverQueue.clear();
    EV_INFO << getFullPath() << ": sent " << bytesSent << " bytes in " << msgsSent << " packets\n";
    EV_INFO << getFullPath() << ": received " << bytesRcvd << " bytes in " << msgsRcvd << " packets\n";
}

} // namespace inet
