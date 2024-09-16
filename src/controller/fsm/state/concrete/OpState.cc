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

OpState::OpState(std::vector<std::shared_ptr<ITransition>> transitions, const char* name) {
	this->transitions = transitions;
	this->name = name;
}

OpState::OpState(const char* name) {
	this->name = name;
}

std::vector<std::shared_ptr<ITransition>>& OpState::getTransitions() {
	return this->transitions;
}

void OpState::setTransitions(std::vector<std::shared_ptr<ITransition>>& transitions) {
	this->transitions = transitions;
}

void OpState::merge(IState* other, std::map<std::string, IState*> states) {
	// Merge state's transitions in a single list
	for(std::shared_ptr<ITransition> otherTransition : other->getTransitions()) {
		bool matched = false;
		for(std::shared_ptr<ITransition> transition : this->getTransitions()) {
			if(transition->equals(otherTransition.get())) {
				// Match found (this transition already exists in the first FSM)
				matched = true;
			}
		}
		if(!matched) {
			// This specific otherTransition does not exist in the current FSM
			// so we add it to the current state transitions (basically to the current FSM)
			const char* otherStateName = otherTransition->getArrivalState()->getName();
			if(states.find(otherStateName) != states.end()) {
				// If the otherTransition we are going to add ends in an arrivalState
				// which matches a state in the current FSM, we substitute it with the corresponding
				// state in the current FSM
				otherTransition->setArrivalState(states.at(otherStateName));
			}
			this->transitions.push_back(otherTransition);
		}
	}
}

IState* OpState::next(IFSM* machine, Packet* msg) {
	IState* nextState = this;
	for(auto transition : this->transitions) {
		if(transition->matchesTransition(msg) && !transition->getDormant()) {
			nextState = transition->execute(msg);
			// This break is necessary because
			// the packet has possibly been deleted
			break;
		}
	}
	return nextState;
}

IState* OpState::next(IFSM* machine, cMessage* event) {
	IState* nextState = this;
	for(auto transition : this->transitions) {
        if(transition->matchesTransition(event) && !transition->getDormant()) {
            nextState = transition->execute(event);
            // This break is necessary because
            // the packet has possibly been deleted
            break;
        }
	}
	return nextState;
}

