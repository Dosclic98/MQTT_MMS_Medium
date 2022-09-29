
#include "Broker.h"


Define_Module(inet::Broker);

void inet::Broker::initialize(int stage)
{
    cSimpleModule::initialize(stage);
    brokerStatus = false;

    if (stage == INITSTAGE_LOCAL) {
        //statistics
        msgsRcvd = msgsSent = bytesRcvd = bytesSent = 0;

        reportBlocked = registerSignal("reportBlockedSignal");
        commandBlocked = registerSignal("commandBlockedSignal");
        reportCompromised = registerSignal("reportCompromisedSignal");
        commandCompromised = registerSignal("commandCompromisedSignal");
        reportCountSignal = registerSignal("reportCountSignal");
        commandCountSignal = registerSignal("commandCountSignal");

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
        brokerQueue.setTakeOwnership(true);
        brokerDepEvent = new cMessage("Broker Dep");
        cModule *node = findContainingNode(this);
        NodeStatus *nodeStatus = node ? check_and_cast_nullable<NodeStatus *>(node->getSubmodule("status")) : nullptr;
        bool isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError("This module doesn't support starting in node DOWN state");
    }
}

void inet::Broker::sendOrSchedule(cMessage *msg, simtime_t delay)
{
    if (delay == 0)
        sendBack(msg);
    else
        scheduleAt(simTime() + delay, msg);
}

void inet::Broker::sendBack(cMessage *msg)
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

bool inet::Broker::maliciousProgram(int topic)
{
    auto p = this->uniform((double)0, (double)1);
    if (topic == 1) { //Command
        if(p < 0.3) { //Edit Command
            emit(commandCompromised, true);
        } else if(p < 0.7) { //Block Command
            if(this->getParentModule()->hasGUI()) this->getParentModule()->bubble("Command Blocked");
            emit(commandBlocked, true);
            return true;
        }
    } else { //Report
        if(p < 0.3) { //Block Report
            if(this->getParentModule()->hasGUI()) this->getParentModule()->bubble("Report Blocked");
            emit(reportBlocked, true);
            return true;
        } else if(p < 0.7) { //Edit Report
            emit(reportCompromised, true);
        }
    }
    return false;
}

void inet::Broker::handleDeparture()
{
    Packet *packet = check_and_cast<Packet *>(brokerQueue.pop());
    int connId = packet->getTag<SocketInd>()->getSocketId();
    ChunkQueue &queue = socketQueue[connId];
    auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
    queue.push(chunk);
    emit(packetReceivedSignal, packet);
    bool doClose = false;

    while (const auto& appmsg = queue.pop<MqttMessage>(b(-1), Chunk::PF_ALLOW_NULLPTR)) {
        msgsRcvd++;
        bytesRcvd += B(appmsg->getChunkLength()).get();
        B requestedBytes = appmsg->getExpectedReplyLength();
        if(appmsg->getMessageKind() == 1) { //SUBSCRIBE
            topicSocketIdMap[appmsg->getTopic()].push_back(connId);
        }
        else if (appmsg->getMessageKind() == 2) { //UNSUBSCRIBE
            topicSocketIdMap[appmsg->getTopic()].remove(connId);
        }
        else { //PUBLISH
            if(appmsg->getTopic() == 0) emit(reportCountSignal, true);
            if(appmsg->getTopic() == 1) emit(commandCountSignal, true);
            if(!par("isDos").boolValue() && this->maliciousProgram(appmsg->getTopic())) {
                    if(appmsg->getServerClose()) doClose = true;
                    break;
            }
            if (requestedBytes > B(0)) {
                if(topicSocketIdMap.count(appmsg->getTopic()) > 0) {
                    for (auto const& subConnId : topicSocketIdMap[appmsg->getTopic()]) {
                        Packet *outPacket = new Packet(packet->getName(), TCP_C_SEND);
                        outPacket->addTag<SocketReq>()->setSocketId(subConnId);
                        const auto& payload = makeShared<MqttMessage>();
                        payload->setTopic(appmsg->getTopic());
                        payload->setMessageKind(0);
                        payload->setChunkLength(requestedBytes);
                        payload->setExpectedReplyLength(B(0));
                        payload->addTag<CreationTimeTag>()->setCreationTime(appmsg->getTag<CreationTimeTag>()
                                ->getCreationTime());
                        outPacket->insertAtBack(payload);
                        sendOrSchedule(outPacket, SimTime(par("replyDelay").intValue(), SIMTIME_MS));
                    }
                } else {
                    if (this->getParentModule()->hasGUI())
                        this->getParentModule()->bubble("This Topic has No Subscribers");
                }
            }
        }
        if (appmsg->getServerClose()) {
            doClose = true;
            break;
        }
    }

    if (doClose) {
        auto request = new Request("close", TCP_C_CLOSE);
        TcpCommand *cmd = new TcpCommand();
        request->addTag<SocketReq>()->setSocketId(connId);
        topicSocketIdMap[0].remove(connId);
        topicSocketIdMap[1].remove(connId);
        request->setControlInfo(cmd);
        sendOrSchedule(request, SimTime(par("replyDelay").intValue(), SIMTIME_MS));
    }
    delete packet;

    brokerStatus = false;

    if (brokerQueue.getLength() > 0) {
        brokerStatus = true;
        scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS),
                            brokerDepEvent);
    }
}

void inet::Broker::handleMessage(cMessage *msg)
{
    if(msg == brokerDepEvent) handleDeparture();
    else if (msg->isSelfMessage()) sendBack(msg);
    else if (msg->getKind() == TCP_I_PEER_CLOSED) {
        // we'll close too, but only after there's surely no message
        // pending to be sent back in this connection
        int connId = check_and_cast<Indication *>(msg)->getTag<SocketInd>()->getSocketId();
        delete msg;
        auto request = new Request("close", TCP_C_CLOSE);
        request->addTag<SocketReq>()->setSocketId(connId);
        if (this->hasGUI()) bubble("Removed");

        //Rimozione ID Connessione dalla lista dei Subscribers.
        topicSocketIdMap[0].remove(connId);
        topicSocketIdMap[1].remove(connId);
        sendOrSchedule(request, SimTime(par("replyDelay").intValue(), SIMTIME_MS));
    }
    else if (msg->getKind() == TCP_I_DATA || msg->getKind() == TCP_I_URGENT_DATA) {
        if(!brokerStatus) {
            brokerStatus = true;
            brokerQueue.insert(msg);
            scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS),
                    brokerDepEvent);
        }
        else brokerQueue.insert(msg);
    }
    else if (msg->getKind() == TCP_I_AVAILABLE)
        socket.processMessage(msg);
    else {
        EV_WARN << "drop msg: " << msg->getName() << ", kind:" << msg->getKind()
                << "(" << cEnum::get("inet::TcpStatusInd")->getStringFor(msg->getKind()) << ")\n";
        delete msg;
    }
}

void inet::Broker::refreshDisplay() const
{
    char buf[64];
    sprintf(buf, "rcvd: %ld pks %ld bytes\nsent: %ld pks %ld bytes", msgsRcvd, bytesRcvd, msgsSent, bytesSent);
    getDisplayString().setTagArg("t", 0, buf);
}

void inet::Broker::finish()
{
    cancelAndDelete(brokerDepEvent);
    brokerQueue.clear();
    EV_INFO << getFullPath() << ": sent " << bytesSent << " bytes in " << msgsSent << " packets\n";
    EV_INFO << getFullPath() << ": received " << bytesRcvd << " bytes in " << msgsRcvd << " packets\n";
}
