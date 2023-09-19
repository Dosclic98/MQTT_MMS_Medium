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

#include "OpTransition.h"

using namespace inet;

OpTransition::OpTransition(const char* expression, IOperation* operation, IController* targetController, IState* arrivalState) {
	this->packetFilter.setExpression(expression);
	this->operation = operation;
	this->targetController = targetController;
	this->arrivalState = arrivalState;
}

bool OpTransition::matchesTransition(Packet* packet) {
	return this->packetFilter.matches(packet);
}

IState* OpTransition::execute(Packet* packet) {
	// TODO Create the operation

	this->targetController->propagate(operation);
	return arrivalState;
}

OpTransition::~OpTransition() {
	// TODO Auto-generated destructor stub
}

