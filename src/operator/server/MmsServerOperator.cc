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
        // Init server status vars
        serverOp = ServerOp::STABLE;

        cEnvir* ev = getSimulation()->getActiveEnvir();
        isLogging = par("isLogging").boolValue();
        if(isLogging) {
        	logger = new MmsServerPacketLogger(ev->getConfigEx()->getActiveRunNumber(), "server", getParentModule()->getIndex(), getIndex());
        }

        // Initializing inherited signals
        resPubSig = registerSignal("serResSig");
        msgPubSig = registerSignal("serMsgSig");
        cmdListener = new MmsOpListener(this);
        // Go up of two levels in the modules hierarchy (the first is the host module)
        getParentModule()->getParentModule()->subscribe("serCmdSig", cmdListener);
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

void MmsServerOperator::handleDeparture(int opId, Packet* packet) {
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
        if(appmsg->getMessageKind() == MMSKind::MEASURE) { // Send data to listeners
			if (requestedBytes > B(0)) {
				sendPacketDeparture(packet->getTag<SocketReq>()->getSocketId(), appmsg->getOriginId(), appmsg->getEvilServerConnId(), B(100), B(0),
						MMSKind::MEASURE, ReqResKind::UNSET, appmsg->getData(), appmsg->getAtkStatus());
			}
			propagate(new MmsServerResult(opId, ResultOutcome::SUCCESS));
        }
        else if (appmsg->getMessageKind() == MMSKind::GENRESP) { // Response to Generic Request
            if (requestedBytes > B(0)) {
                sendPacketDeparture(connId, appmsg->getOriginId(), appmsg->getEvilServerConnId(), requestedBytes, B(0),
                		appmsg->getMessageKind(), appmsg->getReqResKind(), 0, MITMKind::UNMOD);
            }
            propagate(new MmsServerResult(opId, ResultOutcome::SUCCESS));
        }
        else {
            //Bad Request
        	propagate(new MmsServerResult(opId, ResultOutcome::FAIL));
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
        sendOrSchedule(request, SimTime(round(par("replyDelay").doubleValue()), SIMTIME_MS));
    }
}

void MmsServerOperator::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        sendBack(msg);
    }
    else if (msg->getKind() == TCP_I_PEER_CLOSED) {

        // we'll close too, but only after there's surely no message
        // pending to be sent back in this connection
        propagate(check_and_cast<Packet*>(msg));
    }
    else if (msg->getKind() == TCP_I_DATA || msg->getKind() == TCP_I_URGENT_DATA) {
    	if(isLogging) logPacket(check_and_cast<Packet*>(msg), serverOp);
    	propagate(check_and_cast<Packet*>(msg));
    }
    else if (msg->getKind() == TCP_I_AVAILABLE)
        socket.processMessage(msg);
    else {
        // some indication -- ignore
        EV_WARN << "drop msg: " << msg->getName() << ", kind:" << msg->getKind() << "(" << cEnum::get("inet::TcpStatusInd")->getStringFor(msg->getKind()) << ")\n";
        delete msg;
    }
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
}
