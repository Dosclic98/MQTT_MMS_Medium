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

#ifndef SERVER_MMS_MULTI_LISTENERS_FROMSERVERLISTENER_H_
#define SERVER_MMS_MULTI_LISTENERS_FROMSERVERLISTENER_H_

#include "../attacker/ServerEvilOperator.h"

namespace inet {

class FromCliOpListener : public cListener {

public:
	FromCliOpListener();
	FromCliOpListener(ServerEvilOperator* parent);
	virtual ~FromCliOpListener();

	virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject* value, cObject *obj);

	ServerEvilOperator* parent;
};

#endif /* SERVER_MMS_MULTI_LISTENERS_FROMSERVERLISTENER_H_ */

}
