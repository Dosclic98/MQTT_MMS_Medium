
#include "MmsClient.h"

#include "../../message/mms/MmsMessage_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/TimeTag_m.h"

namespace inet {

#define MSGKIND_CONNECT    0
#define MSGKIND_SEND       1

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
        earlySend = false;    // TBD make it parameter
        WATCH(numRequestsToSend);
        WATCH(earlySend);
        WATCH(measureCounter);
        startTime = par("startTime");
        stopTime = par("stopTime");
        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            throw cRuntimeError("Invalid startTime/stopTime parameters");
        timeoutMsg = new cMessage("timer");

        measureAmountEvent = new cMessage("Topic Amount Event");
        measureReceivedCount = registerSignal("measureReceivedCount");
        genericResponseSignal = registerSignal("genericResponseSignal");

        measureCounter = 0;
        isListening = false;
        previousResponseSent = true;
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

void MmsClient::sendRequest()
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
    payload->setServerIndex(this->getIndex());
    if(!isListening) {
    	// Connect kind
        payload->setMessageKind(MMSKind::CONNECT);
        isListening = true;
    }
    else {
        previousResponseSent = true;
        payload->setMessageKind(MMSKind::GENREQ);
        // TODO Add a probability exraction of 0.5 to send a READ or a COMMAND
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
    switch (msg->getKind()) {
        case MSGKIND_CONNECT:
            connect();
            if (earlySend)
                sendRequest();
            break;

        case MSGKIND_SEND:
            sendRequest();
            numRequestsToSend--;
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

    // perform first request if not already done (next one will be sent when reply arrives)
    if (!earlySend)
        sendRequest();

    numRequestsToSend--;
}

void MmsClient::rescheduleOrDeleteTimer(simtime_t d, short int msgKind)
{
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
        if(appmsg->getMessageKind() == 1) {
        	measureCounter++;
        }
        if(appmsg->getMessageKind() == 3) emit(genericResponseSignal, true);
    }
    if (numRequestsToSend > 0) {
        if (previousResponseSent) {
            simtime_t d = simTime() + SimTime(par("thinkTime").intValue(), SIMTIME_MS);
            rescheduleOrDeleteTimer(d, MSGKIND_SEND);
            previousResponseSent = false;
        }
    } else {
        close();
    }

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
