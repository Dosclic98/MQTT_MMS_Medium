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

#ifndef SERVER_MMS_MULTI_LISTENERS_FROMCLIENTLISTENER_H_
#define SERVER_MMS_MULTI_LISTENERS_FROMCLIENTLISTENER_H_

#include "../ClientEvilComp.h"

namespace inet {

class FromClientListener : public cListener {
protected:
	ChunkQueue queue;
public:
	FromClientListener();
	FromClientListener(ClientEvilComp* parent);
	virtual ~FromClientListener();

	virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject* value, cObject *obj);

	ClientEvilComp* parent;
};

}

#endif /* SERVER_MMS_MULTI_LISTENERS_FROMCLIENTLISTENER_H_ */
