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

#include "HttpServerOperator.h"
#include "inet/common/socket/SocketTag_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/packet/Message.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/TimeTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/tcp/TcpCommand_m.h"
#include "../../message/http/HttpMessage_m.h"
#include "../../result/server/HttpServerResult.h"


using namespace inet;

Define_Module(HttpServerOperator);

void HttpServerOperator::initialize(int stage) {
    TcpGenericServerApp::initialize(stage);

    if(stage == INITSTAGE_APPLICATION_LAYER) {

        // Initializing inherited signals
        char strSerCmdSig[30] = "httpSerCmdSig";
        char strSerResPubSig[30] = "httpSerResSig";
        char strSerMsgPubSig[30] = "httpSerMsgSig";

        resPubSig = registerSignal(strSerResPubSig);
        msgPubSig = registerSignal(strSerMsgPubSig);
        cmdListener = new OpListener(this);
        // Go up of two levels in the modules hierarchy (the first is the host module)
        getParentModule()->getParentModule()->subscribe(strSerCmdSig, cmdListener);
    }
}

void HttpServerOperator::sendPacketDeparture(int connId, HttpResponseMessage& resMessage) {
    Packet *outPacket = new Packet("Generic Data", TCP_C_SEND);
    outPacket->addTag<SocketReq>()->setSocketId(connId);
    const auto& payload = makeShared<MmsMessage>();

    //payload->setOriginId(originId);
    //payload->setMessageKind(messageKind);
    //payload->setReqResKind(reqResKind);
    //payload->setChunkLength(requestedBytes);
    //payload->setExpectedReplyLength(replyLength);
    //payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    //payload->setEvilServerConnId(evilConnId);
    //payload->setData(data);
    //payload->setAtkStatus(atkStatus);

    outPacket->insertAtBack(payload);
    sendOrSchedule(outPacket, SimTime(round(par("replyDelay").doubleValue()), SIMTIME_MS));
}

void HttpServerOperator::sendBack(cMessage *msg) {
    TcpGenericServerApp::sendBack(msg);
}

void HttpServerOperator::handleDeparture(int opId, Packet* packet) {
    Enter_Method("Sending HTTP response");
    int connId = packet->getTag<SocketInd>()->getSocketId();
    ChunkQueue &queue = socketQueue[connId];
    auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
    queue.push(chunk);
    emit(packetReceivedSignal, packet);

    while (queue.has<HttpResponseMessage>(b(-1))) {
        const auto& appmsg = queue.pop<HttpResponseMessage>(b(-1));
        msgsRcvd++;
        bytesRcvd += B(appmsg->getChunkLength()).get();

        // TODO See what's done in the MmsServerOperator
    }
    // Take property before deletion
    take(packet);
    delete packet;
}

void HttpServerOperator::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        sendBack(msg);
    }
    else if (msg->getKind() == TCP_I_PEER_CLOSED) {

        // we'll close too, but only after there's surely no message
        // pending to be sent back in this connection
        int connId = check_and_cast<Indication *>(msg)->getTag<SocketInd>()->getSocketId();
        delete msg;
        auto request = new Request("close", TCP_C_CLOSE);
        request->addTag<SocketReq>()->setSocketId(connId);
        sendOrSchedule(request, SimTime(par("replyDelay").doubleValue(), SIMTIME_MS));
    }
    else if (msg->getKind() == TCP_I_DATA || msg->getKind() == TCP_I_URGENT_DATA) {
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


void HttpServerOperator::propagate(IResult* res) {
    emit(this->resPubSig, res);
}

void HttpServerOperator::propagate(Packet* msg) {
    emit(this->msgPubSig, msg);
}
