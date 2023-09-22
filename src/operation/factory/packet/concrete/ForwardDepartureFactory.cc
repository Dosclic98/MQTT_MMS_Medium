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

#include "ForwardDepartureFactory.h"
#include "../../../server/concrete/ForwardDeparture.h"
#include "inet/common/socket/SocketTag_m.h"
#include "inet/common/packet/Message.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/TimeTag_m.h"
#include "../../../../message/mms/MmsMessage_m.h"
#include "inet/transportlayer/contract/tcp/TcpCommand_m.h"

using namespace inet;

void ForwardDepartureFactory::build(Packet* msg) {
	MmsServerController* controller = check_and_cast<MmsServerController*>(this->controller);
	// TODO Eventually manage MMS connect operations
    int connId = msg->getTag<SocketInd>()->getSocketId();
    auto chunk = msg->peekDataAt(B(0), msg->getTotalLength());
    queue.push(chunk);
    bool doClose = false;
    while (queue.has<MmsMessage>(b(-1))) {
    	const auto& appmsg = queue.pop<MmsMessage>(b(-1));
        B requestedBytes = appmsg->getExpectedReplyLength();
        if (appmsg->getServerClose()) {
            doClose = true;
        }

        if(appmsg->getMessageKind() == MMSKind::CONNECT) { // Register a listener
        	controller->clientConnIdList.push_back({connId, appmsg->getEvilServerConnId()});
		}
		else if (appmsg->getMessageKind() == MMSKind::GENREQ) { // Response to Generic Request
			if (requestedBytes > B(0)) {
				Packet *outPacket = new Packet("Generic Data", TCP_C_SEND);
				outPacket->addTag<SocketInd>()->setSocketId(connId);
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

				ForwardDeparture* opDep = new ForwardDeparture(outPacket);
                if(!controller->controllerStatus) {
                	controller->controllerStatus = true;
                	controller->operationQueue.push(opDep);
                	controller->scheduleAt(simTime() + SimTime(controller->par("serviceTime").intValue(), SIMTIME_MS), controller->departureEvent);
                }
                else controller->operationQueue.push(opDep);
			} else {
				// bad request
			}
		}
    }
    delete msg;

    if (doClose) {
        controller->clientConnIdList.remove_if([&](std::pair<int, int>& p) {
            return p.first == connId;
        });
    }
}

ForwardDepartureFactory::ForwardDepartureFactory(MmsServerController* controller) {
	this->controller = controller;
}

ForwardDepartureFactory::~ForwardDepartureFactory() { }

