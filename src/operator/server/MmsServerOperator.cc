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

#include "MmsServerOperator.h"
#include "../../result/server/MmsServerResult.h"
#include "inet/common/socket/SocketTag_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/packet/Message.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/TimeTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/tcp/TcpCommand_m.h"

using namespace inet;

Define_Module(MmsServerOperator);

void MmsServerOperator::initialize(int stage) {
	TcpGenericServerApp::initialize(stage);

    if(stage == INITSTAGE_APPLICATION_LAYER) {
        departureEvent = new cMessage("Server Departure");
        sendDataEvent = new cMessage("Register Data To Send Event");
        serverStatus = false;
        scheduleAt(1, sendDataEvent);

        // Init server status vars
        serverOp = ServerOp::STABLE;

        cEnvir* ev = getSimulation()->getActiveEnvir();
        isLogging = par("isLogging").boolValue();
        if(isLogging) {
        	logger = new MmsServerPacketLogger(ev->getConfigEx()->getActiveRunNumber(), "server", getParentModule()->getIndex(), getIndex());
        }
    }
}

void MmsServerOperator::sendBack(cMessage *msg) {
	Packet* pckt = dynamic_cast<Packet*>(msg);
	if(isLogging) logPacket(pckt, serverOp);

	TcpGenericServerApp::sendBack(msg);
}

void MmsServerOperator::sendPacketDeparture(int connId, msgid_t originId, int evilConnId, B requestedBytes, B replyLength,
		MMSKind messageKind, ReqResKind reqResKind, int data, MITMKind atkStatus) {
    Packet *outPacket = new Packet("Generic Data", TCP_C_SEND);
    outPacket->addTag<SocketReq>()->setSocketId(connId);
    const auto& payload = makeShared<MmsMessage>();
    payload->setOriginId(originId);
    payload->setMessageKind(messageKind);
    payload->setReqResKind(reqResKind);
    payload->setChunkLength(requestedBytes);
    payload->setExpectedReplyLength(replyLength);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    payload->setEvilServerConnId(evilConnId);
    payload->setData(data);
    payload->setAtkStatus(atkStatus);
    outPacket->insertAtBack(payload);
    sendOrSchedule(outPacket, SimTime(round(par("replyDelay").doubleValue()), SIMTIME_MS));
}

void MmsServerOperator::handleDeparture()
{
    Packet *packet = check_and_cast<Packet *>(serverQueue.pop());
    int connId = packet->getTag<SocketInd>()->getSocketId();
    ChunkQueue &queue = socketQueue[connId];
    auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
    queue.push(chunk);
    emit(packetReceivedSignal, packet);
    bool doClose = false;
    while (queue.has<MmsMessage>(b(-1))) {
    	const auto& appmsg = queue.pop<MmsMessage>(b(-1));
        msgsRcvd++;
        bytesRcvd += B(appmsg->getChunkLength()).get();
        B requestedBytes = appmsg->getExpectedReplyLength();
        if(appmsg->getMessageKind() == MMSKind::CONNECT) { // Register a listener
            clientConnIdList.push_back({connId, appmsg->getEvilServerConnId()});
        }
        else if(appmsg->getMessageKind() == MMSKind::MEASURE) { // Send data to listeners
            for (auto const& listener : clientConnIdList) {
                if (requestedBytes > B(0)) {
                    sendPacketDeparture(listener.first, appmsg->getOriginId(), listener.second, B(100), B(0),
                    		MMSKind::MEASURE, ReqResKind::UNSET, appmsg->getData(), appmsg->getAtkStatus());
                }
            }
        }
        else if (appmsg->getMessageKind() == MMSKind::GENREQ) { // Response to Generic Request
            if (requestedBytes > B(0)) {
                sendPacketDeparture(connId, appmsg->getOriginId(), appmsg->getEvilServerConnId(), requestedBytes, B(0),
                		MMSKind::GENRESP, appmsg->getReqResKind(), 0, MITMKind::UNMOD);
            }
        }
        else {
            //Bad Request
        }
        if (appmsg->getServerClose()) {
            doClose = true;
            break;
        }
    }
    delete packet;

    if (doClose) {
        auto request = new Request("close", TCP_C_CLOSE);
        TcpCommand *cmd = new TcpCommand();
        request->addTag<SocketReq>()->setSocketId(connId);
        request->setControlInfo(cmd);
        clientConnIdList.remove_if([&](std::pair<int, int>& p) {
            return p.first == connId;
        });
        sendOrSchedule(request, SimTime(round(par("replyDelay").doubleValue()), SIMTIME_MS));
    }

    if(serverQueue.getLength() > 0) {
        scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS),
                departureEvent);
    } else serverStatus = false;
}

void MmsServerOperator::handleMessage(cMessage *msg)
{
    if (msg == departureEvent) handleDeparture();

    else if(msg == sendDataEvent) {
        auto pkt = new Packet("SendData");
        pkt->addTag<SocketInd>()->setSocketId(-1);
        const auto& payload = makeShared<MmsMessage>();
        payload->setMessageKind(MMSKind::MEASURE);
        payload->setChunkLength(B(100));
        payload->setExpectedReplyLength(B(100));
        payload->setServerClose(false);
        payload->setData(0);
        payload->setAtkStatus(MITMKind::UNMOD);
        pkt->insertAtBack(payload);
        if (!clientConnIdList.empty()) {
            if(!serverStatus) {
                serverStatus = true;
                serverQueue.insert(pkt);
                scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS),
                        departureEvent);
            }
            else serverQueue.insert(pkt);
        }
        scheduleAt(simTime() + SimTime(par("emitInterval").intValue(), SIMTIME_MS), sendDataEvent);
    }

    else if (msg->isSelfMessage()) {
        sendBack(msg);
    }
    else if (msg->getKind() == TCP_I_PEER_CLOSED) {

        // we'll close too, but only after there's surely no message
        // pending to be sent back in this connection
        propagate(check_and_cast<Packet*>(msg));
    }
    else if (msg->getKind() == TCP_I_DATA || msg->getKind() == TCP_I_URGENT_DATA) {
    	if(isLogging) logPacket(check_and_cast<Packet*>(msg), serverOp);
        if(!serverStatus) {
            serverStatus = true;
            serverQueue.insert(msg);
            scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS),
                    departureEvent);
        }
        else serverQueue.insert(msg);
    }
    else if (msg->getKind() == TCP_I_AVAILABLE)
        socket.processMessage(msg);
    else {
        // some indication -- ignore
        EV_WARN << "drop msg: " << msg->getName() << ", kind:" << msg->getKind() << "(" << cEnum::get("inet::TcpStatusInd")->getStringFor(msg->getKind()) << ")\n";
        delete msg;
    }
}

void MmsServerOperator::respondMmsMessage(int opId, int connId, msgid_t originId, int evilConnId, B requestedBytes, B replyLength,
		MMSKind messageKind, ReqResKind reqResKind, int data) {

}

void MmsServerOperator::manageMmsConnection(int opId, MmsMessage* msg) {

}

void MmsServerOperator::generateMmsMeasure(int opId, int connId, msgid_t originId, int evilConnId, B requestedBytes, B replyLength, int data) {
	sendPacketDeparture(connId, originId, evilConnId, requestedBytes, replyLength,
			MMSKind::MEASURE, ReqResKind::UNSET, data, MITMKind::UNMOD);
	propagate(new MmsServerResult(opId, ResultOutcome::SUCCESS));
}

void MmsServerOperator::logPacket(Packet* packet, ServerOp serverOp) {
	ChunkQueue tmpQueue;
	auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
	tmpQueue.push(chunk);
	while (tmpQueue.has<MmsMessage>(b(-1))) {
	    const auto& appmsg = tmpQueue.pop<MmsMessage>(b(-1));
	    logger->log(const_cast<MmsMessage*>(appmsg.get()), serverOp, simTime());
	}
}

void MmsServerOperator::propagate(IResult* res) {
	emit(this->resPubSig, res);
}

void MmsServerOperator::propagate(Packet* msg) {
	emit(this->msgPubSig, msg);
}

MmsServerOperator::~MmsServerOperator() {
	if(isLogging) {
		delete logger;
	}
	cancelAndDelete(departureEvent);
	cancelAndDelete(sendDataEvent);
	serverQueue.clear();
}
