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

#include "MmsServerController.h"
#include "../listener/MmsMsgListener.h"
#include "../listener/MmsResListener.h"
#include "../../operation/server/concrete/ForwardDeparture.h"
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

Define_Module(MmsServerController);

MmsServerController::MmsServerController() {
	// TODO Auto-generated constructor stub
}

MmsServerController::~MmsServerController() {
	cancelAndDelete(departureEvent);
	cancelAndDelete(sendDataEvent);
	serverQueue.clear();
}

void MmsServerController::initialize() {
	cmdPubSig = registerSignal("serCmdSig");

	resListener = new MmsResListener(this);
	msgListener = new MmsMsgListener(this);
	// Subscribe listeners on the right module and signal
	getParentModule()->getParentModule()->subscribe("serResSig", resListener);
	getParentModule()->getParentModule()->subscribe("serMsgSig", msgListener);

    departureEvent = new cMessage("Server Departure");
	sendDataEvent = new cMessage("Register Data To Send Event");
	serverStatus = false;
	scheduleAt(1, sendDataEvent);
}

void MmsServerController::handleMessage(cMessage *msg) {
    if(msg == sendDataEvent) {
    	for (auto const& listener : clientConnIdList) {
			auto pkt = new Packet("SendData");
			// For compatibility not being a real inbound packet
			pkt->addTag<SocketInd>()->setSocketId(-1);
			// Set the outbound socket ID
			pkt->addTag<SocketReq>()->setSocketId(listener.first);
			const auto& payload = makeShared<MmsMessage>();
			payload->setMessageKind(MMSKind::MEASURE);
			payload->setChunkLength(B(100));
			payload->setExpectedReplyLength(B(100));
			payload->setServerClose(false);
			payload->setData(0);
			payload->setAtkStatus(MITMKind::UNMOD);
			payload->setEvilServerConnId(listener.second);
			pkt->insertAtBack(payload);

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
    if(msg == departureEvent) {
    	Packet *packet = check_and_cast<Packet *>(serverQueue.pop());
    	ForwardDeparture* opDep = new ForwardDeparture(idCounter, packet);
    	propagate(opDep);
    	idCounter++;
    	if(serverQueue.getLength() > 0) {
    		scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS), departureEvent);
    	} else serverStatus = false;
    }
}

void MmsServerController::propagate(IOperation* op) {
	emit(this->cmdPubSig, op);
}

void MmsServerController::next(Packet* msg) {
    int connId = msg->getTag<SocketInd>()->getSocketId();
    auto chunk = msg->peekDataAt(B(0), msg->getTotalLength());
    queue.push(chunk);
    bool doClose = false;
    while (queue.has<MmsMessage>(b(-1))) {
    	const auto& appmsg = queue.pop<MmsMessage>(b(-1));
        B requestedBytes = appmsg->getExpectedReplyLength();
        if(appmsg->getMessageKind() == MMSKind::CONNECT) { // Register a listener
            clientConnIdList.push_back({connId, appmsg->getEvilServerConnId()});
        }
        else if (appmsg->getMessageKind() == MMSKind::GENREQ) { // Response to Generic Request
            if (requestedBytes > B(0)) {
                Packet *outPacket = new Packet("Generic Data", TCP_C_SEND);
                outPacket->addTag<SocketReq>()->setSocketId(connId);
                const auto& payload = makeShared<MmsMessage>();
                payload->setOriginId(appmsg->getOriginId());
                payload->setMessageKind(MMSKind::GENRESP);
                payload->setReqResKind(appmsg->getReqResKind());
                payload->setChunkLength(requestedBytes);
                payload->setExpectedReplyLength(B(0));
                payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
                payload->setEvilServerConnId(appmsg->getEvilServerConnId());
                payload->setData(0);
                payload->setAtkStatus(MITMKind::UNMOD);
                outPacket->insertAtBack(payload);

                if(!serverStatus) {
                	serverStatus = true;
                	serverQueue.insert(outPacket);
                	scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS), departureEvent);
                }
                else serverQueue.insert(outPacket);
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
    delete msg;

    if (doClose) {
        clientConnIdList.remove_if([&](std::pair<int, int>& p) {
            return p.first == connId;
        });
    }

}

void MmsServerController::evalRes(IResult* res) {

}

