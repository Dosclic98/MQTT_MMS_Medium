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

#include "OpFSM.h"
#include <queue>

using namespace inet;

IState* OpFSM::getCurrentState() {
	return this->currentState;
}

void OpFSM::setCurrentState(IState* currentState) {
	this->currentState = currentState;
}

IState* OpFSM::next(Packet* msg) {
	IState* nextState = currentState->next(this, msg);
	updateEventScheduling(currentState, nextState);
	this->currentState = nextState;
	return currentState;
}

IState* OpFSM::next(cMessage* event) {
	IState* nextState = currentState->next(this, event);
	updateEventScheduling(currentState, nextState);
	this->currentState = nextState;
	return currentState;
}

void OpFSM::updateEventScheduling(IState* currentState, IState* nextState) {
	if(nextState != currentState) {
		// Deschedule all the eventTransitions related to the currentState and
		// schedule the ones associated to the tmpState
		for(std::shared_ptr<ITransition> transition : currentState->getTransitions()) {
			transition->descheduleSelf();
		}
		for(std::shared_ptr<ITransition> transition : nextState->getTransitions()) {
			transition->scheduleSelf();
		}
	}
}

OpFSM::OpFSM(IController* owner, IState* currentState) {
	this->owner = owner;
	this->currentState = currentState;
	this->initialState = currentState;
	for(std::shared_ptr<ITransition> transition : currentState->getTransitions()) {
		transition->scheduleSelf();
	}
}

OpFSM::~OpFSM() {
	std::set<IState*> visited;
	std::queue<IState*> visiting;
	IState* tmp;

	visiting.push(initialState);
	while(!visiting.empty()) {
		tmp = visiting.front();
		visiting.pop();
		if(visited.find(tmp) == visited.end()) {
			visited.insert(tmp);
			// If tmp has not yet been visited
			std::vector<std::shared_ptr<ITransition>> transitions = tmp->getTransitions();
			for(auto& trans : transitions) {
				IState* next = trans->getArrivalState();
				EV << next->getName() << "\n";
				if(visited.find(next) == visited.end()) {
					visiting.push(next);
				}
			}
		}
	}
	for(IState* visitedState : visited) {
		EV << visitedState->getName() << "\n";
		delete visitedState;
	}

}

