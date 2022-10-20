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


namespace inet {

#define MSGKIND_CONNECT    0
#define MSGKIND_SEND       1

Define_Module(ClientEvilComp);

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

        topicAmountEvent = new cMessage("Topic Amount Event");
        topicAmount = registerSignal("topicAmount");
        genericResponseSignal = registerSignal("genericResponseSignal");
        pcktFromServerSignal = registerSignal("pcktFromServerSignal");
        counter = 0;
        isListening = false;
        previousResponseSent = true;
        // Initialize listener and subscribe to the serverComp forwarding signal
        serverCompListener = new FromClientListener(this);
        sendMsgEvent = new cMessage("Send message event");
        getSimulation()->getSystemModule()->subscribe("pcktFromClientSignal", serverCompListener);
        scheduleAt(simTime() + SimTime(3, SIMTIME_S), topicAmountEvent);
    }
}

void ClientEvilComp::sendRequest()
{
    long requestLength = par("requestLength");
    long replyLength = par("replyLength");
    if (requestLength < 1)
        requestLength = 1;
    if (replyLength < 1)
        replyLength = 1;

    if(!msgQueue.isEmpty()) {
    	MmsMessage* msg = check_and_cast<MmsMessage*>(msgQueue.pop());
        const auto& payload = makeShared<MmsMessage>();
        Packet *packet = new Packet("data");
        payload->setChunkLength(B(requestLength));
        payload->setExpectedReplyLength(B(replyLength));
        payload->setServerClose(msg->getServerClose());
        // TODO This is not correct, the real packet creation time must be set
        payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
        // TODO Maybe store the clients who are are listening based on the messages with kind 0 received
        payload->setMessageKind(msg->getMessageKind());
        /*
        if(!isListening) {
        	// Connect kind
            payload->setMessageKind(0);
            isListening = true;
        }
        else {
            previousResponseSent = true;
            payload->setMessageKind(2);
        }
		*/
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
    }
}

void ClientEvilComp::rescheduleAfterOrDeleteTimer(simtime_t d, short int msgKind) {
	// Necessary because called from another module
	Enter_Method("");
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

    // TODO When the connection is established wait for the ServerComp to forward packets the forward them
    // perform first request if not already done (next one will be sent when reply arrives)
    /*
    if (!earlySend)
    	sendRequest();
	*/
}

void ClientEvilComp::socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) {
    TcpAppBase::socketDataArrived(socket, msg, urgent);

    if (socket->getState() == TcpSocket::LOCALLY_CLOSED) {
        EV_INFO << "reply to last request arrived, closing session\n";
        close();
        return;
    }
    auto chunk = msg->peekDataAt(B(0), msg->getTotalLength());
    queue.push(chunk);
    while (const auto& appmsg = queue.pop<MmsMessage>(b(-1), Chunk::PF_ALLOW_NULLPTR)) {
        counter++;
        if (appmsg->getMessageKind() == 3) emit(genericResponseSignal, true);
    }
    bubble("Message arrived from server");
    // TODO Understand why the ClientEvilComp disconnects after the last connect message is sent (even with 3 clients)
    // When some data arrives, forward it to the ServerEvilComp
    emit(pcktFromServerSignal, msg);
}

}
