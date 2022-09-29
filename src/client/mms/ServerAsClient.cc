
#include "ServerAsClient.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/TimeTag_m.h"

namespace inet {

#define MSGKIND_CONNECT    0
#define MSGKIND_SEND       1

Define_Module(ServerAsClient);

ServerAsClient::~ServerAsClient()
{
    serverQueue.clear();
    cancelAndDelete(timeoutMsg);
    cancelAndDelete(departureEvent);
    cancelAndDelete(sendMeasure);
}

void ServerAsClient::initialize(int stage)
{
    TcpAppBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        numRequestsToSend = 0;
        earlySend = false;    // TBD make it parameter
        WATCH(numRequestsToSend);
        WATCH(earlySend);

        authSent = serverStatus = false;
        departureEvent = new cMessage("Departure Event");
        sendMeasure = new cMessage("Send Measure");

        startTime = par("startTime");
        stopTime = par("stopTime");
        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            throw cRuntimeError("Invalid startTime/stopTime parameters");
        timeoutMsg = new cMessage("timer");

    }
}

void ServerAsClient::handleStartOperation(LifecycleOperation *operation)
{
    simtime_t now = simTime();
    simtime_t start = std::max(startTime, now);
    if (timeoutMsg && ((stopTime < SIMTIME_ZERO) || (start < stopTime) || (start == stopTime && startTime == stopTime))) {
        timeoutMsg->setKind(MSGKIND_CONNECT);
        scheduleAt(start, timeoutMsg);
    }
}

void ServerAsClient::handleStopOperation(LifecycleOperation *operation)
{
    cancelEvent(timeoutMsg);
    if (socket.getState() == TcpSocket::CONNECTED || socket.getState() == TcpSocket::CONNECTING || socket.getState() == TcpSocket::PEER_CLOSED)
        close();
}

void ServerAsClient::handleCrashOperation(LifecycleOperation *operation)
{
    cancelEvent(timeoutMsg);
    if (operation->getRootModule() != getContainingNode(this))
        socket.destroy();
}

void ServerAsClient::sendRequest()
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
    payload->setMessageKind(99);
    packet->insertAtBack(payload);
    sendPacket(packet);

    scheduleAt(simTime() + SimTime(par("emitInterval").intValue(), SIMTIME_MS), sendMeasure);
    //close();
}

void ServerAsClient::handleDeparture() {
    auto appmsg = serverQueue.front();
    serverQueue.pop_front();
    if(appmsg.getMessageKind() == 0) clientConnIdList.push_back(appmsg.getConnId()); //Register Listener
    else if(appmsg.getMessageKind() == 1) { //Send Measures
        for (auto connId : clientConnIdList) {
            long requestLength = par("requestLength");
            long replyLength = par("replyLength");
            const auto& payload = makeShared<MmsMessage>();
            Packet *packet = new Packet("data");
            payload->setChunkLength(B(requestLength));
            payload->setExpectedReplyLength(B(replyLength));
            payload->setServerClose(false);
            payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
            payload->setMessageKind(1);
            payload->setConnId(connId);
            packet->insertAtBack(payload);
            sendPacket(packet);
        }
    }
    else { // Answer to Generic Request
        const auto& payload = makeShared<MmsMessage>();
        Packet *packet = new Packet("data");
        payload->setChunkLength(appmsg.getChunkLength());
        payload->setExpectedReplyLength(appmsg.getExpectedReplyLength());
        payload->setServerClose(false);
        payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
        payload->setMessageKind(3);
        payload->setConnId(appmsg.getConnId());
        packet->insertAtBack(payload);
        sendPacket(packet);
    }

    if(!serverQueue.empty()) scheduleAt(simTime() + SimTime(par("thinkTime").intValue(), SIMTIME_MS), departureEvent);
    else serverStatus = false;
}

void ServerAsClient::handleTimer(cMessage *msg)
{
    if(msg == sendMeasure) {
        MmsMessage msg;
        msg.setMessageKind(1);

        if(!serverStatus) {
            serverQueue.push_back(msg);
            serverStatus = true;
            scheduleAt(simTime() +SimTime(par("thinkTime").intValue(), SIMTIME_MS), departureEvent);
        } else serverQueue.push_back(msg);

        scheduleAt(simTime() + SimTime(par("emitInterval").intValue(), SIMTIME_MS), sendMeasure);
        return;
    }
    if(msg == departureEvent) {
        handleDeparture();
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

void ServerAsClient::socketEstablished(TcpSocket *socket)
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

void ServerAsClient::rescheduleOrDeleteTimer(simtime_t d, short int msgKind)
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

void ServerAsClient::socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent)
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
        MmsMessage msg;
        msg.setMessageKind(appmsg->getMessageKind());
        msg.setConnId(appmsg->getConnId());
        msg.setExpectedReplyLength(appmsg->getExpectedReplyLength());
        msg.setChunkLength(appmsg->getChunkLength());
        msg.setServerClose(appmsg->getServerClose());
        if(!serverStatus) {
            serverStatus = true;
            serverQueue.push_back(msg);
            scheduleAt(simTime() + SimTime(par("thinkTime").intValue(), SIMTIME_MS), departureEvent);
        } else serverQueue.push_back(msg);
    }

}

void ServerAsClient::close()
{
    TcpAppBase::close();
    authSent = false;
    cancelEvent(timeoutMsg);
}

void ServerAsClient::socketClosed(TcpSocket *socket)
{
    TcpAppBase::socketClosed(socket);
    authSent = false;
    // start another session after a delay
    if (timeoutMsg) {
        simtime_t d = simTime() + par("idleInterval");
        rescheduleOrDeleteTimer(d, MSGKIND_CONNECT);
    }
}

void ServerAsClient::socketFailure(TcpSocket *socket, int code)
{
    TcpAppBase::socketFailure(socket, code);
    authSent = false;
    // reconnect after a delay
    if (timeoutMsg) {
        simtime_t d = simTime() + par("reconnectInterval");
        rescheduleOrDeleteTimer(d, MSGKIND_CONNECT);
    }
}

} // namespace inet
