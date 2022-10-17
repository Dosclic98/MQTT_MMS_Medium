//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "ServerEvilComp.h"

namespace inet {

Define_Module(ServerEvilComp);

void ServerEvilComp::initialize(int stage) {
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
        forwardEvent = new cMessage("Message Forward");
        evilServerStatus = false;
        forwardStatus = false;
        forwardQueue = new cQueue();
        serverConnId = -999;

        measureBlockSignal = registerSignal("measureBlockSignal");
        measureCompromisedSignal = registerSignal("measureCompromisedSignal");
        genericRequestBlockSignal = registerSignal("genericRequestBlockSignal");
        genericRequestCompromisedSignal = registerSignal("genericRequestCompromisedSignal");
        genericResponseBlockSignal = registerSignal("genericResponseBlockSignal");
        genericResponseCompromisedSignal = registerSignal("genericResponseCompromisedSignal");
        pcktFromClientSignal = registerSignal("pcktFromClientSignal");
        cModule *node = findContainingNode(this);
        NodeStatus *nodeStatus = node ? check_and_cast_nullable<NodeStatus *>(node->getSubmodule("status")) : nullptr;
        bool isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError("This module doesn't support starting in node DOWN state");
    }
}

void ServerEvilComp::sendPacketDeparture(int connId, B requestedBytes, B replyLength, int messageKind, int clientConnId) {
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
    sendOrSchedule(outPacket, SimTime(par("replyDelay").doubleValue(), SIMTIME_MS));
}

void ServerEvilComp::handleDeparture()
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
        sendOrSchedule(request, SimTime(par("replyDelay").doubleValue(), SIMTIME_MS));
    }

    if(serverQueue.getLength() > 0) {
        scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS),
                departureEvent);
    } else evilServerStatus = false;
}

void ServerEvilComp::handleMessage(cMessage *msg)
{
    if (msg == departureEvent) handleDeparture();
    else if(msg == forwardEvent) handleForward();
    else if (msg->isSelfMessage()) {
        sendBack(msg);
    }
    else if (msg->getKind() == TCP_I_PEER_CLOSED) {

        // we'll close too, but only after there's surely no message
        // pending to be sent back in this connection
    	// TODO Maybe it makes sense to forward the message to the server
        int connId = check_and_cast<Indication *>(msg)->getTag<SocketInd>()->getSocketId();
        delete msg;
        auto request = new Request("close", TCP_C_CLOSE);
        request->addTag<SocketReq>()->setSocketId(connId);
        sendOrSchedule(request, SimTime(par("replyDelay").doubleValue(), SIMTIME_MS));
    }
    else if (msg->getKind() == TCP_I_DATA || msg->getKind() == TCP_I_URGENT_DATA) {
    	// Instead of replying directly to the client we must emit a signal to the internal clients of the evilClient
    	// to forward the message to the real server
    	if(!forwardStatus) {
    		forwardStatus = true;
    		forwardQueue->insert(msg);
    		scheduleAt(simTime() + SimTime(par("forwardDelay").intValue(), SIMTIME_US), forwardEvent);
    	} else forwardQueue->insert(msg);
        /*
    	if(!evilServerStatus) {
            evilServerStatus = true;
            serverQueue.insert(msg);
            scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS), departureEvent);
        }
        else serverQueue.insert(msg);
        */
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

void ServerEvilComp::handleForward() {
	Packet* packet = check_and_cast<Packet*>(forwardQueue->pop());
	emit(pcktFromClientSignal, packet);
	bubble("Sent to internal client!");

	if(forwardQueue->getLength() > 0) {
		scheduleAt(simTime() + SimTime(par("forwardDelay").intValue(), SIMTIME_US), forwardEvent);
	} else forwardStatus = false;
}

void ServerEvilComp::finish()
{
    cancelAndDelete(departureEvent);
    cancelAndDelete(forwardEvent);
    serverQueue.clear();
    forwardQueue->clear();
    delete forwardQueue;
    EV_INFO << getFullPath() << ": sent " << bytesSent << " bytes in " << msgsSent << " packets\n";
    EV_INFO << getFullPath() << ": received " << bytesRcvd << " bytes in " << msgsRcvd << " packets\n";
}

}
