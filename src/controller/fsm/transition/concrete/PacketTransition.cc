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

#include "PacketTransition.h"
#include "../../../../operation/factory/packet/PacketOperationFactory.h"

using namespace inet;

bool PacketTransition::matchesTransition(Packet* packet) {
	return this->packetFilter.matches(packet);
}

IState* PacketTransition::execute(Packet* packet) {
	// Call the builder method
	if(matchesTransition(packet)) {
		PacketOperationFactory* packetOperationFactory = static_cast<PacketOperationFactory*>(this->operationFactory);
		packetOperationFactory->build(packet);
		return arrivalState;
	} else {
		throw std::logic_error("Trying to execute a transition not associated to the referenced packet");
	}
}

bool PacketTransition::matchesTransition(cMessage* event) {
	// It is not an EventTransition
	return false;
}

IState* PacketTransition::execute(cMessage* event) {
	// It is not an EventTransition
	return nullptr;
}

void PacketTransition::scheduleSelf() {
	// Nothing to do (this is not an eRventTransition)
}

void PacketTransition::descheduleSelf() {
	// Nothing to do (this is not an eRventTransition)
}

PacketTransition::PacketTransition(IOperationFactory* operationFactory, IState* arrivalState, const char* expression) {
	PacketOperationFactory* tmpPcktOpFactory = dynamic_cast<PacketOperationFactory*>(operationFactory);
	if(tmpPcktOpFactory == nullptr) {
		throw std::invalid_argument("PacketTransition requires an PacketOperationFactory as operationFactory parameter in the constructor\n");
	}
	this->operationFactory = operationFactory;
	this->arrivalState = arrivalState;

	this->packetFilter.setExpression(expression);
}

PacketTransition::~PacketTransition() {

}

