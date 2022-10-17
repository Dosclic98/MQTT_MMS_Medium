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
        numRequestsToSend = 0;
        earlySend = false;    // TBD make it parameter
        WATCH(numRequestsToSend);
        WATCH(earlySend);
        startTime = par("startTime");
        stopTime = par("stopTime");
        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            throw cRuntimeError("Invalid startTime/stopTime parameters");
        timeoutMsg = new cMessage("timer");

        topicAmountEvent = new cMessage("Topic Amount Event");
        topicAmount = registerSignal("topicAmount");
        genericResponseSignal = registerSignal("genericResponseSignal");
        counter = 0;
        isListening = false;
        previousResponseSent = true;
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

    const auto& payload = makeShared<MmsMessage>();
    Packet *packet = new Packet("data");
    payload->setChunkLength(B(requestLength));
    payload->setExpectedReplyLength(B(replyLength));
    payload->setServerClose(false);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    if(!isListening) {
    	// Connect kind
        payload->setMessageKind(MSGKIND_CONNECT);
        isListening = true;
    }
    else {
        previousResponseSent = true;
        payload->setMessageKind(2);
    }

    packet->insertAtBack(payload);

    sendPacket(packet);
}

void ClientEvilComp::socketEstablished(TcpSocket *socket)
{
    TcpAppBase::socketEstablished(socket);

    // determine number of requests in this session
    numRequestsToSend = par("numRequestsPerSession");
    if (numRequestsToSend < 1)
        numRequestsToSend = 1;

    // perform first request if not already done (next one will be sent when reply arrives)
    if (!earlySend)
        // TODO Remove comment after testing
    	//sendRequest();

    numRequestsToSend--;
}

void ClientEvilComp::socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent)
{
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
    if (numRequestsToSend > 0) {
        if (previousResponseSent) {
            simtime_t d = simTime() + SimTime(par("thinkTime").intValue(), SIMTIME_MS);
            rescheduleAfterOrDeleteTimer(d, MSGKIND_SEND);
            previousResponseSent = false;
        }
    } else {
        close();
    }
}

}
