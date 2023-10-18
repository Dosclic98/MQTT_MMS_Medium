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

#include "MmsClientFSMFactory.h"
#include "../../../../controller/client/MmsClientController.h"
#include "../../state/concrete/OpState.h"
#include "../../operation/OpFSM.h"
#include "../../transition/concrete/EventTransition.h"
#include "../../../../operation/factory/event/concrete/SendMmsConnectFactory.h"
#include "../../../../operation/factory/event/concrete/SendMmsDisconnectFactory.h"
#include "../../../../operation/factory/event/concrete/SendMmsRequestFactory.h"


using namespace inet;

IFSM* MmsClientFSMFactory::build() {
	MmsClientController* cliController = static_cast<MmsClientController*>(this->controller);
	OpState* connectedState = new OpState("CONNECTED");
	OpState* operatingState = new OpState("OPERATING");
	OpState* terminatedState = new OpState("TERMINATED");

	// Create connected transitions
	std::vector<std::shared_ptr<ITransition>> connectedTransitions;
	connectedTransitions.push_back(std::make_shared<EventTransition>(
			new SendMmsConnectFactory(cliController),
			operatingState,
			new cMessage("SENDMEAS", SEND_MMS_CONNECT),
			EventMatchType::Kind
	));
	connectedState->setTransitions(connectedTransitions);

	// Create the operating transitions
	std::vector<std::shared_ptr<ITransition>> operatingTransitions;
	operatingTransitions.push_back(std::make_shared<EventTransition>(
			new SendMmsRequestFactory(cliController),
			operatingState,
			new cMessage("SENDREAD", SEND_MMS_READ),
			EventMatchType::Kind
	));
	operatingTransitions.push_back(std::make_shared<EventTransition>(
			new SendMmsRequestFactory(cliController),
			operatingState,
			new cMessage("SENDCOMMAND", SEND_MMS_COMMAND),
			EventMatchType::Kind
	));
	operatingTransitions.push_back(std::make_shared<EventTransition>(
			new SendMmsDisconnectFactory(cliController),
			connectedState,
			new cMessage("SENDDISCONNECT", SEND_MMS_DISCONNECT),
			EventMatchType::Kind
	));
	operatingState->setTransitions(operatingTransitions);

	return new OpFSM(this->controller, connectedState);
}

MmsClientFSMFactory::~MmsClientFSMFactory() {
	// TODO Auto-generated destructor stub
}

MmsClientFSMFactory::MmsClientFSMFactory(MmsClientController* controller) {
	this->controller = controller;
}

