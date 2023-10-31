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
	updateEventSchedulingAfterExecution(currentState, nextState);
	this->currentState = nextState;
	return currentState;
}

IState* OpFSM::next(cMessage* event) {
	IState* nextState = currentState->next(this, event);
	updateEventSchedulingAfterExecution(currentState, nextState);
	this->currentState = nextState;
	return currentState;
}

void OpFSM::updateEventSchedulingAfterExecution(IState* currentState, IState* nextState) {
	if(nextState != currentState) {
		// Deschedule all the eventTransitions related to the currentState and
		// schedule the ones associated to the nextState (rescheduling of events
		// if the arrival state is the same as the current one is managed in the execute())
		for(std::shared_ptr<ITransition> transition : currentState->getTransitions()) {
			transition->descheduleSelf();
		}
		for(std::shared_ptr<ITransition> transition : nextState->getTransitions()) {
			transition->scheduleSelf();
		}
	}
}

void OpFSM::updateEventSchedulingAfterMerge(IState* currentState) {
	// When a merge is performed we must schedule all the new events introduced by EventTransition(s)
	for(std::shared_ptr<ITransition> transition : currentState->getTransitions()) {
		if(!transition->isScheduled()) {
			transition->scheduleSelf();
		}
	}
}

std::set<IState*> OpFSM::getStates() {
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

	return visited;
}

std::map<std::string, IState*> OpFSM::getStatesMap() {
	std::map<std::string, IState*> statesMap;
	std::set<IState*> states = getStates();

	for(IState* state : states) {
		statesMap.insert({std::string(state->getName()), state});
	}
	return statesMap;
}

IController* OpFSM::getOwner() {
	return owner;
}

void OpFSM::merge(IFSM* other) {
	if(this->owner != other->getOwner()) {
		throw std::invalid_argument("Trying to merge two OpFSM with different owners");
	}
	// Determine the common states (the ones with the same name)
	std::map<std::string, IState*> states = this->getStatesMap();
	std::map<std::string, IState*> otherStates = other->getStatesMap();
	std::set<IState*> toDelete;
	for(std::pair<std::string, IState*> pair : states) {
		IState* state = pair.second;
		if(otherStates.find(state->getName()) != otherStates.end()) {
			IState* otherState = otherStates.at(state->getName());
			state->merge(otherState, states);
			// Insert the otherState in the deletion set
			toDelete.insert(otherState);
		}
	}
	// Delete the merged states
	for(IState* delState : toDelete) { delete delState; }
	// When the merge is completed schedule the new events
	this->updateEventSchedulingAfterMerge(this->currentState);
}

OpFSM::OpFSM(IController* owner, IState* currentState, bool scheduleOnBuild) {
	this->owner = owner;
	this->currentState = currentState;
	this->initialState = currentState;
	if(scheduleOnBuild) {
		for(std::shared_ptr<ITransition> transition : currentState->getTransitions()) {
			transition->scheduleSelf();
		}
	}
}

OpFSM::~OpFSM() {
	std::set<IState*> states = this->getStates();
	for(IState* state : states) {
		EV << state->getName() << "\n";
		delete state;
	}

}

