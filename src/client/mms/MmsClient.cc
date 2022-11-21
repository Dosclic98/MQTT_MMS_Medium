
#include "MmsClient.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/TimeTag_m.h"

namespace inet {

#define MSGKIND_CONNECT    				0
#define MSGKIND_SEND_READ       		1
#define MSGKIND_SEND_COMMAND       		2
#define MSGKIND_RES_TIMEOUT	   			3

Define_Module(MmsClient);

MmsClient::~MmsClient()
{
    cancelAndDelete(measureAmountEvent);
    cancelAndDelete(timeoutMsg);
}

void MmsClient::initialize(int stage)
{
    TcpAppBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        numRequestsToSend = 0;
        WATCH(numRequestsToSend);
        WATCH(measureCounter);
        startTime = par("startTime");
        stopTime = par("stopTime");
        resTimeout = par("resTimeoutInterval");
        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            throw cRuntimeError("Invalid startTime/stopTime parameters");
        timeoutMsg = new cMessage("timer");

        measureAmountEvent = new cMessage("Topic Amount Event");
        measureReceivedCount = registerSignal("measureReceivedCount");
        readSentSignal = registerSignal("readSentSignal");
        commandSentSignal = registerSignal("commandSentSignal");
        genericResponseSignal = registerSignal("genericResponseSignal");
        genericResponseTimeoutSignal = registerSignal("genericResponseTimeoutSignal");

        measureCounter = 0;
        isListening = false;
        scheduleAt(simTime() + SimTime(measureAmountEventDelay, SIMTIME_S), measureAmountEvent);
    }
}

void MmsClient::handleStartOperation(LifecycleOperation *operation)
{
    simtime_t now = simTime();
    simtime_t start = std::max(startTime, now);
    if (timeoutMsg && ((stopTime < SIMTIME_ZERO) || (start < stopTime) || (start == stopTime && startTime == stopTime))) {
        timeoutMsg->setKind(MSGKIND_CONNECT);
        scheduleAt(start, timeoutMsg);
    }
}

void MmsClient::handleStopOperation(LifecycleOperation *operation)
{
    cancelEvent(timeoutMsg);
    if (socket.getState() == TcpSocket::CONNECTED || socket.getState() == TcpSocket::CONNECTING || socket.getState() == TcpSocket::PEER_CLOSED)
        close();
}

void MmsClient::handleCrashOperation(LifecycleOperation *operation)
{
    cancelEvent(timeoutMsg);
    if (operation->getRootModule() != getContainingNode(this))
        socket.destroy();
}

void MmsClient::sendRequest(MMSKind kind, ReqResKind reqKind)
{
    long requestLength = par("requestLength");
    long replyLength = par("replyLength");
    if (requestLength < 1)
        requestLength = 1;
    if (replyLength < 1)
        replyLength = 1;

    const auto& payload = makeShared<MmsMessage>();
    Packet *packet = new Packet("data");
    payload->setOriginId(packet->getId());
    payload->setChunkLength(B(requestLength));
    payload->setExpectedReplyLength(B(replyLength));
    payload->setServerClose(false);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    payload->setServerIndex(this->getIndex());
    EV << "Index: " << this->getIndex();
    if(!isListening && kind == MMSKind::CONNECT) {
    	// Connect kind
        payload->setMessageKind(kind);
        isListening = true;
    }
    else {
        payload->setMessageKind(kind);
        // Send a Read or a Command
        payload->setReqResKind(reqKind);

        if(reqKind == ReqResKind::READ) {
        	emit(readSentSignal, true);
        } else if(reqKind == ReqResKind::COMMAND) {
        	emit(commandSentSignal, true);
        }

        // Add the timeout event for the respective response
        cMessage* resTimeoutMsg = new cMessage("Response timeout");
        resTimeoutMsg->setKind(MSGKIND_RES_TIMEOUT);
        resTimeoutMap.insert({payload->getOriginId(), resTimeoutMsg});
        scheduleAt(simTime() + SimTime(resTimeout, SIMTIME_S), resTimeoutMsg);
    }

    packet->insertAtBack(payload);

    sendPacket(packet);
}

void MmsClient::handleTimer(cMessage *msg)
{
    if (msg == measureAmountEvent) {
        emit(measureReceivedCount, measureCounter);
        measureCounter = 0;
        scheduleAt(simTime() + SimTime(measureAmountEventDelay, SIMTIME_S), measureAmountEvent);
        return;
    }
    simtime_t dRead = 0;
    simtime_t dCommand = 0;
    switch (msg->getKind()) {
        case MSGKIND_CONNECT:
            connect();
            delete msg;
            break;

        case MSGKIND_SEND_READ:
            sendRequest(MMSKind::GENREQ, ReqResKind::READ);
            numRequestsToSend--;
            delete msg;
            // Schedule a Read send
			dRead = simTime() + SimTime(par("sendReadInterval").intValue(), SIMTIME_S);
			rescheduleOrDeleteTimer(dRead, MSGKIND_SEND_READ);
            break;

        case MSGKIND_SEND_COMMAND:
            sendRequest(MMSKind::GENREQ, ReqResKind::COMMAND);
            numRequestsToSend--;
            delete msg;
            // Schedule a Command send
            dCommand = simTime() + SimTime(par("sendCommandInterval").intValue(), SIMTIME_S);
            rescheduleOrDeleteTimer(dCommand, MSGKIND_SEND_COMMAND);
            break;

        case MSGKIND_RES_TIMEOUT:
        	for(auto &i : resTimeoutMap) {
        		if (i.second == msg) {
        			resTimeoutMap.erase(i.first);
        			delete msg;
        			// Emit signal for generic response timeout
        			emit(genericResponseTimeoutSignal, true);
        			break;
        		}
        	}
        	break;

        default:
            throw cRuntimeError("Invalid timer msg: kind=%d", msg->getKind());
    }
}

void MmsClient::socketEstablished(TcpSocket *socket)
{
    TcpAppBase::socketEstablished(socket);

    // determine number of requests in this session
    numRequestsToSend = par("numRequestsPerSession");
    if (numRequestsToSend < 1)
        numRequestsToSend = 1;


    // Send the register for measure MMS message
    sendRequest();
    numRequestsToSend--;

    // Schedule a Read send
    simtime_t dRead = simTime() + SimTime(par("sendReadInterval").intValue(), SIMTIME_S);
    rescheduleOrDeleteTimer(dRead, MSGKIND_SEND_READ);
    // // Schedule a Command send
    simtime_t dCommand = simTime() + SimTime(par("sendCommandInterval").intValue(), SIMTIME_S);
    rescheduleOrDeleteTimer(dCommand, MSGKIND_SEND_COMMAND);
}

void MmsClient::rescheduleOrDeleteTimer(simtime_t d, short int msgKind)
{
    //cancelEvent(timeoutMsg);
	timeoutMsg = new cMessage("timer");

    if (stopTime < SIMTIME_ZERO || d < stopTime) {
        timeoutMsg->setKind(msgKind);
        scheduleAt(d, timeoutMsg);
    }
    else {
        delete timeoutMsg;
        timeoutMsg = nullptr;
    }
}

void MmsClient::socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent)
{
    if (socket->getState() == TcpSocket::LOCALLY_CLOSED) {
        EV_INFO << "reply to last request arrived, closing session\n";
        close();
        return;
    }
    auto chunk = msg->peekDataAt(B(0), msg->getTotalLength());
    queue.push(chunk);
    while (const auto& appmsg = queue.pop<MmsMessage>(b(-1), Chunk::PF_ALLOW_NULLPTR)) {
        if(appmsg->getMessageKind() == MMSKind::MEASURE) {
        	measureCounter++;
        }
        if(appmsg->getMessageKind() == MMSKind::GENRESP) {
        	emit(genericResponseSignal, true);
        	if(resTimeoutMap.find(appmsg->getOriginId()) != resTimeoutMap.end()) {
        		cMessage* tmpTimeout = resTimeoutMap[appmsg->getOriginId()];
        		resTimeoutMap.erase(appmsg->getOriginId());
        		cancelAndDelete(tmpTimeout);
        	}
        }
    }
/*
    if (numRequestsToSend > 0) {
        if (previousResponseSent) {
            simtime_t d = simTime() + SimTime(par("thinkTime").intValue(), SIMTIME_MS);
            rescheduleOrDeleteTimer(d, MSGKIND_SEND);
            previousResponseSent = false;
        }
    } else {
        close();
    }
*/
    TcpAppBase::socketDataArrived(socket, msg, urgent);
}

void MmsClient::close()
{
    TcpAppBase::close();

    cancelEvent(timeoutMsg);
}

void MmsClient::socketClosed(TcpSocket *socket)
{
    TcpAppBase::socketClosed(socket);

    // start another session after a delay
    if (timeoutMsg) {
        simtime_t d = simTime() + par("idleInterval");
        rescheduleOrDeleteTimer(d, MSGKIND_CONNECT);
    }
}

void MmsClient::socketFailure(TcpSocket *socket, int code)
{
    TcpAppBase::socketFailure(socket, code);

    // reconnect after a delay
    if (timeoutMsg) {
        simtime_t d = simTime() + par("reconnectInterval");
        rescheduleOrDeleteTimer(d, MSGKIND_CONNECT);
    }
}

} // namespace inet
