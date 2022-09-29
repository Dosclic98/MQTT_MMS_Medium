
#include "MqttClient.h"

#include "../../message/mqtt/MqttMessage_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/TimeTag_m.h"

namespace inet {

#define MSGKIND_CONNECT    0
#define MSGKIND_SEND       1

Define_Module(MqttClient);

MqttClient::~MqttClient()
{
    cancelAndDelete(timeoutMsg);
    cancelAndDelete(pktRangeCountAdv);
}

void MqttClient::initialize(int stage)
{
    TcpAppBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        numRequestsToSend = 0;
        earlySend = false;    // TBD make it parameter
        WATCH(numRequestsToSend);
        WATCH(earlySend);

        mqttPacketLifeTimeSignal = registerSignal("mqttPacketLifeTimeSignal");
        topicAmountSignal = registerSignal("topicAmountSignal");
        topicSentSignal = registerSignal("topicSentSignal");
        startTime = par("startTime");
        stopTime = par("stopTime");
        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            throw cRuntimeError("Invalid startTime/stopTime parameters");
        timeoutMsg = new cMessage("timer");
        isSubscribed = false;
        pktRangeCountAdv = new cMessage("3 Seconds Count");
        scheduleAt(simTime() + SimTime(3, SIMTIME_S), pktRangeCountAdv);
        counter = 0;
    }
}

void MqttClient::handleStartOperation(LifecycleOperation *operation)
{
    simtime_t now = simTime();
    simtime_t start = std::max(startTime, now);
    if (timeoutMsg && ((stopTime < SIMTIME_ZERO) || (start < stopTime) || (start == stopTime && startTime == stopTime))) {
        timeoutMsg->setKind(MSGKIND_CONNECT);
        scheduleAt(start, timeoutMsg);
    }
}

void MqttClient::handleStopOperation(LifecycleOperation *operation)
{
    cancelEvent(timeoutMsg);
    if (socket.getState() == TcpSocket::CONNECTED || socket.getState() == TcpSocket::CONNECTING || socket.getState() == TcpSocket::PEER_CLOSED)
        close();
}

void MqttClient::handleCrashOperation(LifecycleOperation *operation)
{
    cancelEvent(timeoutMsg);
    if (operation->getRootModule() != getContainingNode(this))
        socket.destroy();
}

void MqttClient::sendRequest()
{
    long requestLength = par("requestLength");
    long replyLength = par("replyLength");
    if (requestLength < 1)
        requestLength = 1;
    if (replyLength < 1)
        replyLength = 1;

    const auto& payload = makeShared<MqttMessage>();
    Packet *packet = new Packet("data");
    payload->setChunkLength(B(requestLength));
    payload->setExpectedReplyLength(B(replyLength));
    payload->setServerClose(false);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());

    if (!isSubscribed) {
        isSubscribed = true;
        payload->setMessageKind(1);  // [1 subscription]
        payload->setTopic(par("subTopic").intValue());
    } else {
        payload->setMessageKind(0); // [0 publish]
        payload->setTopic(par("pubTopic").intValue());
    }
    packet->insertAtBack(payload);

    if (numRequestsToSend > 0) {
        emit(topicSentSignal, true);
        sendPacket(packet);
        simtime_t d = simTime() + SimTime(par("thinkTime").intValue(), SIMTIME_MS);
        rescheduleOrDeleteTimer(d, MSGKIND_SEND);
    } else {
        delete packet;
        //close();

    }
}

void MqttClient::handleTimer(cMessage *msg)
{
    if(msg == pktRangeCountAdv) {
        emit(topicAmountSignal, counter);
        counter = 0;
        scheduleAt(simTime() + SimTime(3, SIMTIME_S), pktRangeCountAdv);
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

void MqttClient::socketEstablished(TcpSocket *socket)
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

void MqttClient::rescheduleOrDeleteTimer(simtime_t d, short int msgKind)
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

void MqttClient::socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent)
{
    if (socket->getState() == TcpSocket::LOCALLY_CLOSED) {
        EV_INFO << "reply to last request arrived, closing session\n";
        close();
        return;
    }

    auto chunk = msg->peekDataAt(B(0), msg->getTotalLength());

    queue.push(chunk);

    while (const auto& appmsg = queue.pop<MqttMessage>(b(-1), Chunk::PF_ALLOW_NULLPTR)) {
        emit(mqttPacketLifeTimeSignal, ( simTime() - appmsg->getTag<CreationTimeTag>()->getCreationTime()) );
        counter++;
    }

    TcpAppBase::socketDataArrived(socket, msg, urgent);
}

void MqttClient::close()
{
    TcpAppBase::close();
    isSubscribed = false;
    cancelEvent(timeoutMsg);
}

void MqttClient::socketClosed(TcpSocket *socket)
{
    TcpAppBase::socketClosed(socket);
    isSubscribed = false;
    // start another session after a delay
    if (timeoutMsg) {
        simtime_t d = simTime() + par("idleInterval");
        rescheduleOrDeleteTimer(d, MSGKIND_CONNECT);
    }
}

void MqttClient::socketFailure(TcpSocket *socket, int code)
{
    TcpAppBase::socketFailure(socket, code);
    isSubscribed = false;
    // reconnect after a delay
    if (timeoutMsg) {
        simtime_t d = simTime() + par("reconnectInterval");
        rescheduleOrDeleteTimer(d, MSGKIND_CONNECT);
    }
}

} // namespace inet
