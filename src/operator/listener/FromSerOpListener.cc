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

#include "FromSerOpListener.h"
#include "inet/common/TimeTag_m.h"


namespace inet {

#define MSGKIND_CONNECT    0
#define MSGKIND_SEND       1

FromSerOpListener::FromSerOpListener() {
	// TODO Auto-generated constructor stub
	messageCopier = new MmsMessageCopier();
}

FromSerOpListener::FromSerOpListener(ClientEvilOperator* parent) {
	this->parent = parent;
	FromSerOpListener();
}

FromSerOpListener::~FromSerOpListener() {
	// TODO Auto-generated destructor stub
}

void FromSerOpListener::receiveSignal(cComponent *source, simsignal_t signalID, cObject* value, cObject *obj){
	this->parent->bubble("Packet received from fwd signal!");

	Packet* pckt = check_and_cast<Packet*>(value);
	// Propagate packet to the attacker's controller
	parent->propagate(pckt);
}

}
