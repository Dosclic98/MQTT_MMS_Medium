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


const char* OpState::getName() {
	return this->name;
}

std::vector<std::shared_ptr<ITransition>>& OpState::getTransitions() {
	return this->transitions;
}

void OpState::setTransitions(std::vector<std::shared_ptr<ITransition>>& transitions) {
	this->transitions = transitions;
}

IState* OpState::next(IFSM* machine, Packet* msg) {
	IState* nextState = this;
	for(auto transition : this->transitions) {
		if(transition->matchesTransition(msg)) {
			nextState = transition->execute(msg);
			// This break is necessary because
			// the packet has possibly been deleted
			break;
		}
	}
	return nextState;
}

IState* OpState::next(IFSM* machine, cEvent* event) {
	IState* nextState = this;
	for(auto transition : this->transitions) {
			if(transition->matchesTransition(event)) {
				nextState = transition->execute(event);
				// This break is necessary because
				// the packet has possibly been deleted
				break;
			}
	}
	return nextState;
}

