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

using namespace inet;

IFSM* MmsClientFSMFactory::build() {
	MmsClientController* cliController = static_cast<MmsClientController*>(this->controller);
	OpState* connectedState = new OpState();
	OpState* operatingState = new OpState();
	OpState* terminatedState = new OpState();

	// Create connected transitions
	std::vector<ITransition*> connectedTransitions;
	connectedTransitions.push_back(new EventTransition(
			new SendMmsConnectFactory(cliController),
			operatingState,
			new cMessage("SENDMEAS", SEND_MMS_CONNECT),
			EventMatchType::Kind
	));
	connectedState->setTransitions(connectedTransitions);

	// Create the operating transitions
	std::vector<ITransition*> operatingTransitions;
	operatingTransitions.push_back(new EventTransition(
			new SendMmsRequestFactory(cliController),
			operatingState,
			new cMessage("SENDREAD", SEND_MMS_READ),
			EventMatchType::Kind
	));
	operatingTransitions.push_back(new EventTransition(
			new SendMmsRequestFactory(cliController),
			operatingState,
			new cMessage("SENDCOMMAND", SEND_MMS_COMMAND),
			EventMatchType::Kind
	));
	operatingTransitions.push_back(new EventTransition(
			new SendMmsDisconnectFactory(cliController),
			terminatedState,
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

