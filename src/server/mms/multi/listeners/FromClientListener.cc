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

#include "FromClientListener.h"
#include "../../../../message/mms/MmsMessage_m.h"
#include "inet/common/TimeTag_m.h"


namespace inet {

#define MSGKIND_CONNECT    0
#define MSGKIND_SEND       1

FromClientListener::FromClientListener() {
	// TODO Auto-generated constructor stub
}

FromClientListener::FromClientListener(ClientEvilComp* parent) {
	this->parent = parent;
	FromClientListener();
}

FromClientListener::~FromClientListener() {
	// TODO Auto-generated destructor stub
}

void FromClientListener::receiveSignal(cComponent *source, simsignal_t signalID, cObject* value, cObject *obj){
	this->parent->bubble("Packet received from fwd signal!");
	Packet* pckt = check_and_cast<Packet*>(value);
    auto chunk = pckt->peekDataAt(B(0), pckt->getTotalLength());
    queue.push(chunk);
    while (const auto& appmsg = queue.pop<MmsMessage>(b(-1), Chunk::PF_ALLOW_NULLPTR)) {
		// Add to the forward to the server queue
		MmsMessage* msg = new MmsMessage();
		msg->setMessageKind(appmsg->getMessageKind());
		msg->setConnId(appmsg->getConnId());
		msg->setExpectedReplyLength(appmsg->getExpectedReplyLength());
		msg->setChunkLength(appmsg->getChunkLength());
		msg->setEvilServerConnId(appmsg->getEvilServerConnId());
		msg->setServerClose(appmsg->getServerClose());
		msg->addTag<CreationTimeTag>()->setCreationTime(appmsg->getTag<CreationTimeTag>()->getCreationTime());
		msg->setServerIndex(appmsg->getServerIndex());

		// Signal if a generic request gets blocked or compromised
		double p = this->parent->uniform(0.0, 1.0);
		if(appmsg->getMessageKind() == 2) {
			if (p < 0.2) { // Block
				this->parent->emit(this->parent->genericRequestBlockSignal, true);
				delete pckt;
				delete msg;
				return;
			} else if (p < 0.8) { // Compromise
				this->parent->emit(this->parent->genericRequestCompromisedSignal, true);
			}
		}

		if(this->parent->previousResponseSent && this->parent->getConnectionState() == TcpSocket::CONNECTED) {
			this->parent->msgQueue.insert(msg);
			simtime_t d = simTime() + SimTime(round(this->parent->par("thinkTime").doubleValue()), SIMTIME_MS);
			// We suppose the client is already connected to the server, so when the data arrives we send it (MSG_KIND_SEND)
			this->parent->rescheduleAfterOrDeleteTimer(d, MSGKIND_SEND);
			this->parent->previousResponseSent = false;
		} else {
			this->parent->msgQueue.insert(msg);
		}

    }
}

}
