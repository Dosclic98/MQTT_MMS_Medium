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

using namespace inet;

bool PacketTransition::matchesTransition(Packet* packet) {
	return this->packetFilter.matches(packet);
}

IState* PacketTransition::execute(Packet* packet) {
	// Call the builder method

}

bool PacketTransition::matchesTransition(cEvent* event) {
	// It is not an EventTransition
	return false;
}

IState* PacketTransition::execute(cEvent* event) {
	// It is not an EventTransition
	return nullptr;
}

PacketTransition::PacketTransition(IOperation* operation, IController* targetController, IState* arrivalState, const char* expression) {
	this->operation = operation;
	this->targetController = targetController;
	this->arrivalState = arrivalState;

	this->packetFilter.setExpression(expression);
}

PacketTransition::~PacketTransition() {

}

