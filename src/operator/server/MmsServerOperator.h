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

#ifndef __TX_MEDIUM_EXP_MMSSERVEROPERATOR_H_
#define __TX_MEDIUM_EXP_MMSSERVEROPERATOR_H_

#include <omnetpp.h>
#include "inet/applications/tcpapp/TcpGenericServerApp.h"
#include "../../utils/logger/server/MmsServerPacketLogger.h"
#include "../IOperator.h"
#include "../listener/MmsOpListener.h"
#include "inet/common/packet/ChunkQueue.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"

namespace inet {

enum ServerOp {
	STABLE = 0,
	UNSTABLE = 1
};

class MmsServerOperator : public TcpGenericServerApp, public IOperator {
protected:
	// Logging pacchetti inviati/ricevuti
	bool isLogging;
	MmsServerPacketLogger* logger;

	// Status vars
	ServerOp serverOp;
	int numComprMsgs;


	virtual void sendBack(cMessage *msg) override;

	virtual void initialize(int stage) override;
	virtual void handleMessage(cMessage *msg) override;
	//--
	virtual void sendPacketDeparture(int connId, msgid_t originId, int evilConnId, B requestedBytes, B replyLength,
			MMSKind messageKind, ReqResKind reqResKind, int data, MITMKind atkStatus);

	void logPacket(Packet* packet, ServerOp serverOp);

	virtual ~MmsServerOperator();
public:
	virtual void handleDeparture(int opId, Packet* packet);

	virtual void propagate(IResult* res) override;
	virtual void propagate(Packet* msg) override;
};

} // namespace inet

#endif
