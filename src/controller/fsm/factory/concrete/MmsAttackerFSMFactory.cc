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
#include "../../transition/concrete/EventTransition.h"
#include "../../transition/concrete/PacketTransition.h"
#include "../../../../operation/factory/packet/concrete/ForwardMmsMessageToServerFactory.h"
#include "../../../../operation/factory/packet/concrete/ForwardMmsMessageToClientFactory.h"
#include "../../../../operation/factory/event/concrete/SendTcpConnectAtkFactory.h"
#include "../../concrete/OpFSM.h"

using namespace inet;

// Builds the initial Attacker's FSM (enriched then by the Attack graph execution)
IFSM* MmsAttackerFSMFactory::build() {
	MmsAttackerController* atkController = static_cast<MmsAttackerController*>(this->controller);
	OpState* unconnectedState = new OpState("UNCONNECTED");
	OpFSM* fsm = new OpFSM(controller, unconnectedState);

	return fsm;
}

MmsAttackerFSMFactory::MmsAttackerFSMFactory(MmsAttackerController* controller) {
	this->controller = controller;
}

MmsAttackerFSMFactory::~MmsAttackerFSMFactory() {
	// TODO Auto-generated destructor stub
}

