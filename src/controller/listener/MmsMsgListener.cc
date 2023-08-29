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

#include "MmsMsgListener.h"

using namespace inet;

MmsMsgListener::MmsMsgListener(IController* parent) {
	this->parent = parent;
}

MmsMsgListener::~MmsMsgListener() {
	// TODO Auto-generated destructor stub
}

void MmsMsgListener::receiveSignal(omnetpp::cComponent* source, omnetpp::simsignal_t signalID, omnetpp::cObject* value, omnetpp::cObject *obj) {
	Packet* msg = check_and_cast<Packet*>(value);
	parent->next(msg);
}

