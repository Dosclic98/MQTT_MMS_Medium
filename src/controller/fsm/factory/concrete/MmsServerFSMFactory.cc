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

#include "MmsServerFSMFactory.h"
#include "../../../../controller/IController.h"
#include "../../../../controller/server/MmsServerController.h"
#include "../../state/concrete/OpState.h"
#include "../../operation/OpFSM.h"
#include "../../transition/concrete/EventTransition.h"
#include "../../transition/concrete/PacketTransition.h"
#include "../../../../operation/factory/packet/concrete/ForwardDepartureFactory.h"
#include "../../../../operation/factory/event/concrete/GenerateMeasuresFactory.h"

using namespace inet;

IFSM* MmsServerFSMFactory::build() {
	MmsServerController* serController = static_cast<MmsServerController*>(this->controller);
	OpState* operativeState = new OpState("OPERATIVE");

	std::vector<ITransition*> operativeTransitions;
	operativeTransitions.push_back(new PacketTransition(
		new ForwardDepartureFactory(serController),
		operativeState,
		"content.messageKind == 0" // If messageKind == MMSKind::CONNECT
	));
	/* TODO Understand why adding this gives a segfault
	operativeTransitions.push_back(new PacketTransition(
		new ForwardDepartureFactory(serController),
		operativeState,
		"content.messageKind == 2" // If messageKind == MMSKind::GENREQ
	));
	*/
	operativeTransitions.push_back(new EventTransition(
		new GenerateMeasuresFactory(serController),
		operativeState,
		serController->getSendMeasuresEvent(),
		EventMatchType::Ref
	));

	operativeState->setTransitions(operativeTransitions);
	return new OpFSM(serController, operativeState);
}

MmsServerFSMFactory::MmsServerFSMFactory(MmsServerController* controller) {
	this->controller = controller;
}

MmsServerFSMFactory::~MmsServerFSMFactory() {
	// TODO Auto-generated destructor stub
}

