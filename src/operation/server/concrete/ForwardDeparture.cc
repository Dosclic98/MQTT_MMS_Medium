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

#include "ForwardDeparture.h"
#include "../../../operator/server/MmsServerOperator.h"

using namespace inet;

ForwardDeparture::ForwardDeparture(Packet* packet) {
	this->id = ++ForwardDeparture::idCounter;
	this->msg = packet;
}

ForwardDeparture::~ForwardDeparture() {
	// TODO Auto-generated destructor stub
}

void ForwardDeparture::execute() {
	MmsServerOperator* oper = check_and_cast<MmsServerOperator*>(operatorOwner);
	oper->handleDeparture(this->id, this->msg);
}


