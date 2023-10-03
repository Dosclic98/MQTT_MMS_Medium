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

#include "OpState.h"
#include "../../IFSM.h"
#include "../../../../operation/IOperation.h"
#include "../../../IController.h"
#include "../../transition/concrete/EventTransition.h"
#include "../../transition/concrete/PacketTransition.h"

using namespace inet;

OpState::~OpState() {
	// TODO Auto-generated destructor stub
}

OpState::OpState(std::vector<ITransition*> transitions, const char* name) {
	this->transitions = transitions;
	this->name = name;
}

OpState::OpState(const char* name) {
	this->name = name;
}


const char* OpState::getName() {
	return this->name;
}

std::vector<ITransition*> OpState::getTransitions() {
	return this->transitions;
}

void OpState::setTransitions(std::vector<ITransition*> transitions) {
	this->transitions = transitions;
}

IState* OpState::next(IFSM* machine, Packet* msg) {
	IState* nextState = this;
	for(ITransition* transition : this->transitions) {
		PacketTransition* pcktTrans = dynamic_cast<PacketTransition*>(transition);
		if(pcktTrans != nullptr) {
			// This is an event transition
			if(pcktTrans->matchesTransition(msg)) {
				// Transition found
				nextState = pcktTrans->execute(msg);
			}
		}
	}
	return nextState;
}

IState* OpState::next(IFSM* machine, cEvent* event) {
	IState* nextState = this;
	for(ITransition* transition : this->transitions) {
		EventTransition* evTrans = dynamic_cast<EventTransition*>(transition);
		if(evTrans != nullptr) {
			// This is an event transition
			if(evTrans->matchesTransition(event)) {
				// Transition found
				nextState = evTrans->execute(event);
			}
		}
	}
	return nextState;
}

