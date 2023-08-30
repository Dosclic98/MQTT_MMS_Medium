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

#include "FromCliOpListener.h"

namespace inet {

FromCliOpListener::FromCliOpListener()
{
	// TODO Auto-generated constructor stub

}

FromCliOpListener::FromCliOpListener(ServerEvilOperator* parent) {
	this->parent = parent;
	FromCliOpListener();
}

FromCliOpListener::~FromCliOpListener() {
	// TODO Auto-generated destructor stub
}

void FromCliOpListener::receiveSignal(cComponent *source, simsignal_t signalID, cObject* value, cObject *obj) {
	this->parent->bubble("Message from server!");
	Packet* pckt = check_and_cast<Packet*>(value);
	if(!this->parent->evilServerStatus) {
		this->parent->evilServerStatus = true;
		this->parent->serverQueue.insert(pckt);
		this->parent->scheduleAt(simTime() + SimTime(this->parent->par("serviceTime").intValue(), SIMTIME_MS), this->parent->departureEvent);
	}
	else this->parent->serverQueue.insert(pckt);
}

}

