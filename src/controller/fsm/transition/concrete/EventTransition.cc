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

bool EventTransition::matchesTransition(cEvent* event, EventMatchType matchType) {
	switch(matchType) {
		case EventMatchType::Kind: {
			return check_and_cast<cMessage*>(this->event)->getKind() == check_and_cast<cMessage*>(event)->getKind();
			break;
		}
		case EventMatchType::Ref: {
			return this->event == event;
			break;
		}
	}
	return this->event == event;
}

IState* EventTransition::execute(cEvent* event, EventMatchType matchType) {
	if(matchesTransition(event, matchType)) {
		EventOperationFactory* evOpFactory = static_cast<EventOperationFactory*>(operationFactory);
		evOpFactory->build(event);
		return arrivalState;
	} else {
		throw std::logic_error("Trying to execute a transition not associated to the referenced event");
	}
}

EventTransition::EventTransition(IOperationFactory* operationFactory, IState* arrivalState, cEvent* event) {
	EventOperationFactory* tmpEvOpFactory = dynamic_cast<EventOperationFactory*>(operationFactory);
	if(tmpEvOpFactory == nullptr) throw std::invalid_argument("EventTransition requires an EventOperationFactory as operationFactory parameter in the constructor\n");
	this->operationFactory = operationFactory;
	this->arrivalState = arrivalState;
	this->event = event;
}

EventTransition::~EventTransition() {

}

