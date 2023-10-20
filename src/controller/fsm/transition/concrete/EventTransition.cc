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
#include "../../../../operation/factory/event/EventOperationFactory.h"

using namespace inet;

bool EventTransition::matchesTransition(Packet* packet) {
	// This is not a PacketTransition
	return false;
}

IState* EventTransition::execute(Packet* packet) {
	// This is not a PacketTransition
	return nullptr;
}

bool EventTransition::matchesTransition(cMessage* event) {
	switch(this->matchType) {
		case EventMatchType::Kind: {
			return this->event->getKind() == event->getKind();
			break;
		}
		case EventMatchType::Ref: {
			EV << this->event << "\n";
			EV << event << "\n";
			return this->event == event;
			break;
		}
	}
	return this->event == event;
}

IState* EventTransition::execute(cMessage* event) {
	if(matchesTransition(event)) {
		EventOperationFactory* evOpFactory = static_cast<EventOperationFactory*>(operationFactory);
		evOpFactory->build(event);
		// Reschedule event triggering the transition
		this->scheduleSelf();
		return arrivalState;
	} else {
		throw std::logic_error("Trying to execute a transition not associated to the referenced event");
	}
}

void EventTransition::scheduleSelf() {
	// Reschedule the event causing the transition to trigger;
	EventOperationFactory* evOpFactory = static_cast<EventOperationFactory*>(operationFactory);
	IController* controller = evOpFactory->getController();
	controller->scheduleEvent(event, delay);
}

void EventTransition::descheduleSelf() {
	EventOperationFactory* evOpFactory = static_cast<EventOperationFactory*>(operationFactory);
	IController* controller = evOpFactory->getController();
	controller->descheduleEvent(event);
}

EventTransition::EventTransition(IOperationFactory* operationFactory, IState* arrivalState, cMessage* event,
		EventMatchType matchType, simtime_t delay) {
	EventOperationFactory* tmpEvOpFactory = dynamic_cast<EventOperationFactory*>(operationFactory);
	if(tmpEvOpFactory == nullptr) throw std::invalid_argument("EventTransition requires an EventOperationFactory as operationFactory parameter in the constructor\n");
	this->operationFactory = operationFactory;
	this->arrivalState = arrivalState;
	this->event = event;
	this->matchType = matchType;
	this->delay = delay;
}

EventTransition::~EventTransition() {
	this->descheduleSelf();
	if(this->matchType == EventMatchType::Kind) {
		delete this->event;
	}
}

