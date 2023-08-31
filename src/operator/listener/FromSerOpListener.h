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

#ifndef SERVER_MMS_MULTI_LISTENERS_FROMSEROPLISTENER_H_
#define SERVER_MMS_MULTI_LISTENERS_FROMSEROPLISTENER_H_

#include "../attacker/ClientEvilOperator.h"
#include "../../message/mms/MmsMessage_m.h"
#include "../../utils/factories/mms/MmsMessageCopier.h"

namespace inet {

class FromSerOpListener : public cListener {
protected:
	ChunkQueue queue;
	MmsMessageCopier* messageCopier;

public:
	FromSerOpListener();
	FromSerOpListener(ClientEvilOperator* parent);
	virtual ~FromSerOpListener();

	virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject* value, cObject *obj);

	ClientEvilOperator* parent;
};

}

#endif /* SERVER_MMS_MULTI_LISTENERS_FROMCLIENTLISTENER_H_ */
