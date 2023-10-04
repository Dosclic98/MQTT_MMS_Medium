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

#include "GenerateMeasuresFactory.h"
#include "../../../server/concrete/ForwardDeparture.h"
#include "inet/common/socket/SocketTag_m.h"
#include "inet/common/packet/Message.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/TimeTag_m.h"
#include "../../../../message/mms/MmsMessage_m.h"
#include "inet/transportlayer/contract/tcp/TcpCommand_m.h"

using namespace inet;

void GenerateMeasuresFactory::build(omnetpp::cEvent* event) {
	MmsServerController* controller = check_and_cast<MmsServerController*>(this->controller);
	for (auto const& listener : controller->getMmsSubscriberList()) {
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

		ForwardDeparture* opDep = new ForwardDeparture(pkt);
        controller->enqueueNSchedule(opDep);
	}
    controller->scheduleNextMeasureSend();
}

GenerateMeasuresFactory::GenerateMeasuresFactory(MmsServerController* controller) {
	this->controller = controller;
}

GenerateMeasuresFactory::~GenerateMeasuresFactory() { }

