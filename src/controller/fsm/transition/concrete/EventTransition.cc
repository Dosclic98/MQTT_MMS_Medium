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

#include "EventTransition.h"

using namespace inet;

bool EventTransition::matchesTransition(Packet* packet) {
	// This is not a PacketTransition
	return false;
}

IState* EventTransition::execute(Packet* packet) {
	// This is not a PacketTransition
	return nullptr;
}

bool EventTransition::matchesTransition(cEvent* event) {
	return this->event == event;
}

IState* EventTransition::execute(cEvent* event) {
	if(matchesTransition(event)) {
		this->targetController->propagate(operation);
		return arrivalState;
	} else {
		throw std::logic_error("Trying to execute a transition not associated to the referenced event");
	}
}

EventTransition::EventTransition(IOperation* operation, IController* targetController, IState* arrivalState, cEvent* event) {
	this->operation = operation;
	this->targetController = targetController;
	this->arrivalState = arrivalState;
	this->event = event;
}

EventTransition::~EventTransition() {

}

