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

#include "MmsAttackerFSMFactory.h"
#include "../../../../controller/IController.h"
#include "../../../../controller/attacker/MmsAttackerController.h"
#include "../../state/concrete/OpState.h"
#include "../../operation/OpFSM.h"
#include "../../transition/concrete/EventTransition.h"
#include "../../transition/concrete/PacketTransition.h"
#include "../../../../operation/factory/packet/concrete/ForwardMmsMessageToServerFactory.h"
#include "../../../../operation/factory/packet/concrete/ForwardMmsMessageToClientFactory.h"
#include "../../../../operation/factory/event/concrete/SendTcpConnectAtkFactory.h"

using namespace inet;

IFSM* MmsAttackerFSMFactory::build() {
	MmsAttackerController* atkController = static_cast<MmsAttackerController*>(this->controller);
	OpState* unconnectedState = new OpState("UNCONNECTED");
	OpState* opState = new OpState("OPERATIVE");

	std::vector<std::shared_ptr<ITransition>> unconnectedTransitions;
	unconnectedTransitions.push_back(std::make_shared<EventTransition>(
		new SendTcpConnectAtkFactory(atkController),
		opState,
		new cMessage("TCPCONNECT", MSGKIND_CONNECT),
		EventMatchType::Kind
	));
	unconnectedState->setTransitions(unconnectedTransitions);

	std::vector<std::shared_ptr<ITransition>> operativeTransitions;
	operativeTransitions.push_back(std::make_shared<PacketTransition>(
		new ForwardMmsMessageToClientFactory(atkController),
		opState,
		"content.messageKind == 1 || content.messageKind == 3" // messageKind == MMSKind::MEASURE || messageKind == MMSKind::GENRESP
	));
	operativeTransitions.push_back(std::make_shared<PacketTransition>(
		new ForwardMmsMessageToServerFactory(atkController),
		opState,
		"content.messageKind == 0 || content.messageKind == 2" // messageKind == MMSKind::CONNECT || messageKind == MMSKind::GENREQ
	));

	opState->setTransitions(operativeTransitions);
	OpFSM* fsm = new OpFSM(controller, unconnectedState);

	return fsm;
}

MmsAttackerFSMFactory::MmsAttackerFSMFactory(MmsAttackerController* controller) {
	this->controller = controller;
}

MmsAttackerFSMFactory::~MmsAttackerFSMFactory() {
	// TODO Auto-generated destructor stub
}

