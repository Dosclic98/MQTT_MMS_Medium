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


#include "ClientEvilComp.h"
#include "./listeners/FromClientListener.h"
#include "../../../message/mms/MmsMessage_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/socket/SocketTag_m.h"


namespace inet {

#define MSGKIND_CONNECT    0
#define MSGKIND_SEND       1

Define_Module(ClientEvilComp);

ClientEvilComp::~ClientEvilComp() {
	cancelAndDelete(sendMsgEvent);
	msgQueue.clear();
}

void ClientEvilComp::initialize(int stage)
{
    TcpAppBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        earlySend = false;    // TBD make it parameter
        numRequestsToSend = 1000;
        WATCH(earlySend);
        WATCH(numRequestsToSend);
        startTime = par("startTime");
        stopTime = par("stopTime");
        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            throw cRuntimeError("Invalid startTime/stopTime parameters");
        timeoutMsg = new cMessage("timer");
        messageCopier = new MmsMessageCopier();

        genericFakeReqResSignal = registerSignal("genericFakeReqResSignal");
        pcktFromServerSignal = registerSignal("pcktFromServerSignal");
        readRequestBlockSignal = registerSignal("readRequestBlockSignal");
        readRequestCompromisedSignal = registerSignal("readRequestCompromisedSignal");
        commandRequestBlockSignal = registerSignal("commandRequestBlockSignal");
        commandRequestCompromisedSignal = registerSignal("commandRequestCompromisedSignal");
        measureBlockSignal = registerSignal("measureBlockSignal");
        measureCompromisedSignal = registerSignal("measureCompromisedSignal");
        readResponseBlockSignal = registerSignal("readResponseBlockSignal");
        readResponseCompromisedSignal = registerSignal("readResponseCompromisedSignal");
        commandResponseBlockSignal = registerSignal("commandResponseBlockSignal");
        commandResponseCompromisedSignal = registerSignal("commandResponseCompromisedSignal");

    	readResponseBlockProb = par("readResponseBlockProb").doubleValue();
    	readResponseCompromisedProb = par("readResponseCompromisedProb").doubleValue();
    	commandResponseBlockProb = par("commandResponseBlockProb").doubleValue();
    	commandResponseCompromisedProb = par("commandResponseCompromisedProb").doubleValue();

    	readRequestBlockProb = par("readRequestBlockProb").doubleValue();
    	readRequestCompromisedProb = par("readRequestCompromisedProb").doubleValue();
    	commandRequestBlockProb = par("commandRequestBlockProb").doubleValue();
    	commandRequestCompromisedProb = par("commandRequestCompromisedProb").doubleValue();

    	measureBlockProb = par("measureBlockProb").doubleValue();
    	measureCompromisedProb = par("measureCompromisedProb").doubleValue();

        previousResponseSent = true;
        // Initialize listener and subscribe to the serverComp forwarding signal
        serverCompListener = new FromClientListener(this);
        sendMsgEvent = new cMessage("Send message event");
        char strSig[30];
        sprintf(strSig, "pcktFromClientSignal-%d", this->getIndex());
        getContainingNode(this)->subscribe(strSig, serverCompListener);
    }
}

void ClientEvilComp::sendRequest() {
    if(!msgQueue.isEmpty()) {
    	MmsMessage* msg = check_and_cast<MmsMessage*>(msgQueue.pop());
        const auto& payload = messageCopier->copyMessage(msg, true);
        Packet *packet = new Packet("data");
        packet->insertAtBack(payload);

        sendPacket(packet);
        if(!msgQueue.isEmpty()) {
        	simtime_t d = simTime() + SimTime(round(par("thinkTime").doubleValue()), SIMTIME_MS);
        	// We suppose the client is already connected to the server, so when the data arrives we send it (MSG_KIND_SEND)
        	rescheduleAfterOrDeleteTimer(d, MSGKIND_SEND);
        	previousResponseSent = false;
        } else {
        	previousResponseSent = true;
        }

        delete msg;
    }
}


void ClientEvilComp::rescheduleAfterOrDeleteTimer(simtime_t d, short int msgKind) {
	// Necessary because called from another module
    cancelEvent(timeoutMsg);

    if (stopTime < SIMTIME_ZERO || d < stopTime) {
        timeoutMsg->setKind(msgKind);
        scheduleAt(d, timeoutMsg);
    }
    else {
        delete timeoutMsg;
        timeoutMsg = nullptr;
    }
}

void ClientEvilComp::handleTimer(cMessage *msg)
{
    switch (msg->getKind()) {
        case MSGKIND_CONNECT:
            connect();
            if (earlySend)
                sendRequest();
            break;

        case MSGKIND_SEND:
            sendRequest();
            break;

        default:
            throw cRuntimeError("Invalid timer msg: kind=%d", msg->getKind());
    }
}

void ClientEvilComp::socketEstablished(TcpSocket *socket)
{
    TcpAppBase::socketEstablished(socket);

    // Forward the packets waiting to be forwarded to the server
    if(!msgQueue.isEmpty() && previousResponseSent) {
    	simtime_t d = simTime() + SimTime(round(par("thinkTime").doubleValue()), SIMTIME_MS);
		rescheduleAfterOrDeleteTimer(d, MSGKIND_SEND);
		previousResponseSent = false;
    }
}

int ClientEvilComp::getConnectionState() {
	return socket.getState();
}

void ClientEvilComp::socketDataArrived(TcpSocket *socket, Packet *pckt, bool urgent) {
    if (socket->getState() == TcpSocket::LOCALLY_CLOSED) {
        EV_INFO << "reply to last request arrived, closing session\n";
        close();
        return;
    }
    auto chunk = pckt->peekDataAt(B(0), pckt->getTotalLength());
    queue.push(chunk);
    while (queue.has<MmsMessage>(b(-1))) {
    	const auto& appmsg = queue.pop<MmsMessage>(b(-1));
    	if(appmsg->getMessageKind() == MMSKind::GENRESP && appmsg->getEvilServerConnId() == -1) {
            // Emit signal for generic fake Req Res
    		emit(genericFakeReqResSignal, true);
            TcpAppBase::socketDataArrived(socket, pckt, urgent);
            return;
    	}

		const auto& msg = messageCopier->copyMessage(appmsg.get(), appmsg->getEvilServerConnId(), true);
		Packet *packet = new Packet("data");

		packet->insertAtBack(msg);
		packet->addTag<SocketInd>()->setSocketId(appmsg->getEvilServerConnId());
		bubble("Sent to internal client!");
		EV_INFO << "Conn ID:" << msg->getEvilServerConnId() << "\n";

		MMSKind messageKind = appmsg->getMessageKind();
		ReqResKind reqResKind = appmsg->getReqResKind();
	    double p = this->uniform(0.0, 1.0);
	    if (messageKind == MMSKind::MEASURE) {
	        if (p < measureBlockProb) { //Block
	        	bubble("Measure blocked");
	            emit(measureBlockSignal, true);
	            delete packet;
	            TcpAppBase::socketDataArrived(socket, pckt, urgent);
	            return;
	        } else if (p < measureCompromisedProb){ //Compromise
	        	bubble("Measure compromised");
	            emit(measureCompromisedSignal, true);
	        } else {
	        	bubble("Measure arrived from server");
	        }
	    } else if (messageKind == MMSKind::GENRESP) {
	    	if(reqResKind == ReqResKind::READ) {
		        if (p < readResponseBlockProb) { // Block
		        	bubble("Read response blocked");
		            emit(readResponseBlockSignal, true);
		            delete packet;
		            TcpAppBase::socketDataArrived(socket, pckt, urgent);
		            return;
		        } else if (p < readResponseCompromisedProb) { // Compromise
		        	bubble("Read response compromised");
		            emit(readResponseCompromisedSignal, true);
		        } else {
		        	bubble("Read response arrived from server");
		        }
	    	} else if(reqResKind == ReqResKind::COMMAND) {
		        if (p < commandResponseBlockProb) { // Block
		        	bubble("Command response blocked");
		            emit(commandResponseBlockSignal, true);
		            delete packet;
		            TcpAppBase::socketDataArrived(socket, pckt, urgent);
		            return;
		        } else if (p < commandResponseCompromisedProb) { // Compromise
		        	bubble("Command response compromised");
		            emit(commandResponseCompromisedSignal, true);
		        } else {
		        	bubble("Command response arrived from server");
		        }
	    	}
	    }

		emit(pcktFromServerSignal, packet);
    }

    // Leave this call at the end because it deletes the packet
    TcpAppBase::socketDataArrived(socket, pckt, urgent);
}

}
