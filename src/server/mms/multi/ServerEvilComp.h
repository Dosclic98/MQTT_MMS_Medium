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

#ifndef __TX_MEDIUM_EXP_SERVEREVILCOMP_H_
#define __TX_MEDIUM_EXP_SERVEREVILCOMP_H_

#include <omnetpp.h>
#include "inet/applications/tcpapp/TcpGenericServerApp.h"
#include "../../../message/mms/MmsMessage_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/socket/SocketTag_m.h"

namespace inet {

class FromServerListener;
/**
 * TODO - Generated class
 */
class INET_API ServerEvilComp : public TcpGenericServerApp {
	public:
		//Gestione servitore e coda
		bool evilServerStatus;
		cQueue serverQueue;
		cMessage* departureEvent;
		virtual ~ServerEvilComp();

	protected:

		//Gestione servitore e coda
		bool forwardStatus;
		cQueue* forwardQueue;
		cMessage* forwardEvent;
		ChunkQueue tmpQueue;

		// Gestione segnali canali interni
		FromServerListener* clientCompListener;
		simsignal_t* pcktFromClientSignal;


	protected:
		virtual void initialize(int stage) override;
		virtual void handleMessage(cMessage *msg) override;
		virtual void finish() override;
		virtual void handleDeparture();
		virtual void sendPacketDeparture(int connId, msgid_t originId, simtime_t fakeCreationTime, B requestedBytes, B replyLength, MMSKind messageKind, ReqResKind reqResKind, int clientConnId);
		void handleForward();
};

} // namespace inet

#endif
