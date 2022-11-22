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
#include "./listeners/FromServerListener.h"

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

        int numApps = getContainingNode(this)->par("numApps").intValue();
        pcktFromClientSignal = new simsignal_t[numApps-1];
        for(int i = 0; i < numApps-1; i++) {
        	char strSig[30];
        	sprintf(strSig, "pcktFromClientSignal-%d", i);
        	pcktFromClientSignal[i] = registerSignal(strSig);
        }
        // Initialize the listener for the incoming server messages
        clientCompListener = new FromServerListener(this);
        getContainingNode(this)->subscribe("pcktFromServerSignal", clientCompListener);
        cModule *node = findContainingNode(this);
        NodeStatus *nodeStatus = node ? check_and_cast_nullable<NodeStatus *>(node->getSubmodule("status")) : nullptr;
        bool isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError("This module doesn't support starting in node DOWN state");
    }
}

void ServerEvilComp::sendPacketDeparture(int connId, msgid_t originId, simtime_t fakeCreationTime, B requestedBytes, B replyLength, MMSKind messageKind, ReqResKind reqResKind, int clientConnId) {
    Packet *outPacket = new Packet("Generic Data", TCP_C_SEND);
    outPacket->addTag<SocketReq>()->setSocketId(connId);
    const auto& payload = makeShared<MmsMessage>();
    payload->setOriginId(originId);
    payload->setMessageKind(messageKind);
    payload->setReqResKind(reqResKind);
    payload->setChunkLength(requestedBytes);
    payload->setExpectedReplyLength(replyLength);
    payload->addTag<CreationTimeTag>()->setCreationTime(fakeCreationTime);
    payload->setConnId(clientConnId);
    payload->setServerClose(false);
    outPacket->insertAtBack(payload);
    sendOrSchedule(outPacket, SimTime(par("replyDelay").doubleValue(), SIMTIME_MS));
}

void ServerEvilComp::handleDeparture() {
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
        // I set the chunk length as response length because we must forward the data
        B requestedBytes = appmsg->getChunkLength();
        if(appmsg->getMessageKind() == MMSKind::MEASURE) sendPacketDeparture(appmsg->getConnId(), appmsg->getOriginId(), appmsg->getTag<CreationTimeTag>()->getCreationTime(), requestedBytes, B(0), MMSKind::MEASURE, ReqResKind::UNSET, -1);
        else if (appmsg->getMessageKind() == MMSKind::GENRESP) { //Generic Response From Server
            if (requestedBytes > B(0)) sendPacketDeparture(appmsg->getConnId(), appmsg->getOriginId(), appmsg->getTag<CreationTimeTag>()->getCreationTime(), requestedBytes, B(0), MMSKind::GENRESP, appmsg->getReqResKind(), -1);
        }
        else { /* Bad Request, not present in MITM */}
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

// Extracts the packet from the queue and unpack the MMS encapsulated packet inserting the connId
// before forwarding it to the ClientEvilComp
void ServerEvilComp::handleForward() {
	Packet* pckt = check_and_cast<Packet*>(forwardQueue->pop());
	int connId = pckt->getTag<SocketInd>()->getSocketId();
	auto chunk = pckt->peekDataAt(B(0), pckt->getTotalLength());
	ChunkQueue &queue = socketQueue[connId];
	queue.push(chunk);
	while (const auto& appmsg = queue.pop<MmsMessage>(b(-1), Chunk::PF_ALLOW_NULLPTR)) {
		const auto& msg = makeShared<MmsMessage>();
		Packet *packet = new Packet("data");
		msg->setOriginId(appmsg->getOriginId());
		msg->setMessageKind(appmsg->getMessageKind());
		msg->setReqResKind(appmsg->getReqResKind());
		msg->setConnId(connId);
		msg->setExpectedReplyLength(appmsg->getExpectedReplyLength());
		msg->setChunkLength(appmsg->getChunkLength());
		msg->setEvilServerConnId(connId);
		msg->setServerClose(appmsg->getServerClose());
		msg->addTag<CreationTimeTag>()->setCreationTime(appmsg->getTag<CreationTimeTag>()->getCreationTime());
		msg->setServerIndex(appmsg->getServerIndex());
		packet->insertAtBack(msg);
		emit(pcktFromClientSignal[appmsg->getServerIndex()], packet);
		bubble("Sent to internal client!");
		EV_INFO << "Conn ID:" << msg->getEvilServerConnId() << "\n";

		if(forwardQueue->getLength() > 0) {
			scheduleAt(simTime() + SimTime(par("forwardDelay").intValue(), SIMTIME_US), forwardEvent);
		} else forwardStatus = false;
	}
	delete pckt;
}

void ServerEvilComp::finish() {
    EV_INFO << getFullPath() << ": sent " << bytesSent << " bytes in " << msgsSent << " packets\n";
    EV_INFO << getFullPath() << ": received " << bytesRcvd << " bytes in " << msgsRcvd << " packets\n";
}

ServerEvilComp::~ServerEvilComp() {
    cancelAndDelete(departureEvent);
    cancelAndDelete(forwardEvent);
    serverQueue.clear();
    forwardQueue->clear();
    delete forwardQueue;
    delete pcktFromClientSignal;
}

}
