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
	this->currentState = currentState->next(this, msg);
	return currentState;
}

IState* OpFSM::next(cEvent* event) {
	this->currentState = currentState->next(this, event);
	return currentState;
}

OpFSM::OpFSM(IController* owner, IState* currentState) {
	this->owner = owner;
	this->currentState = currentState;
	this->initialState = currentState;
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
			std::vector<ITransition*> transitions = tmp->getTransitions();
			for(ITransition* trans : transitions) {
				IState* next = trans->getArrivalState();
				EV << next->getName() << "\n";
				if(visited.find(next) == visited.end()) {
					visiting.push(next);
				}
				delete trans;
			}
		}
	}
	for(IState* visitedState : visited) {
		EV << visitedState->getName() << "\n";
		delete visitedState;
	}

}

