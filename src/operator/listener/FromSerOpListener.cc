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
	fakeGenReqThresh = parent->par("fakeGenReqThresh").intValue();
	numGenReq = 0;
	FromSerOpListener();
}

FromSerOpListener::~FromSerOpListener() {
	// TODO Auto-generated destructor stub
}

void FromSerOpListener::receiveSignal(cComponent *source, simsignal_t signalID, cObject* value, cObject *obj){
	this->parent->bubble("Packet received from fwd signal!");

	Packet* pckt = check_and_cast<Packet*>(value);
    auto chunk = pckt->peekDataAt(B(0), pckt->getTotalLength());
    queue.push(chunk);
    while (queue.has<MmsMessage>(b(-1))) {
    	const auto& appmsg = queue.pop<MmsMessage>(b(-1));
		if(appmsg->getMessageKind() == MMSKind::GENREQ) {
			numGenReq++;
			// If at least a certain number of generic requests has been sent
			// generate a fake request for the server
			if(numGenReq >= fakeGenReqThresh) {
				numGenReq = 0;

				// Add to the forward to the server queue
				MmsMessage* msg = new MmsMessage();
				// Useless, could also not be set
				msg->setOriginId(pckt->getId());
				msg->setMessageKind(MMSKind::GENREQ);
				// Set to read for now
				msg->setReqResKind(ReqResKind::READ);
				msg->setConnId(appmsg->getConnId());
				msg->setExpectedReplyLength(appmsg->getExpectedReplyLength());
				msg->setChunkLength(appmsg->getChunkLength());
				// Set to -1 so the ClientEvilComp Will not forward it to the server
				msg->setEvilServerConnId(-1);
				msg->setServerClose(false);
				msg->addTag<CreationTimeTag>()->setCreationTime(simTime());
				msg->setData(0);
				msg->setAtkStatus(MITMKind::FAKEGEN);
				if(parent->isLogging) parent->logger->log(msg, EvilStateName::FULL, simTime());

				enqueueNSchedule(msg);
			}
		}

		// Add to the forward to the server queue
		MmsMessage* msg = messageCopier->copyMessageNorm(appmsg.get(), true);
		// Signal if a generic request gets blocked or compromised
		double p = this->parent->uniform(0.0, 1.0);
		if(appmsg->getMessageKind() == MMSKind::GENREQ) {
			if(appmsg->getReqResKind() == ReqResKind::READ) {
				if (p < this->parent->readRequestBlockProb) { // Block
					this->parent->emit(this->parent->readRequestBlockSignal, true);
					msg->setAtkStatus(MITMKind::BLOCK);
					if(parent->isLogging) parent->logger->log(msg, EvilStateName::FULL, simTime());
					delete pckt;
					delete msg;
					return;
				} else if (p - this->parent->readRequestBlockProb < this->parent->readRequestCompromisedProb) { // Compromise
					msg->setAtkStatus(MITMKind::COMPR);
					msg->setData(9);
					if(parent->isLogging) parent->logger->log(msg, EvilStateName::FULL, simTime());
					this->parent->emit(this->parent->readRequestCompromisedSignal, true);
				} else {
					if(parent->isLogging) parent->logger->log(msg, EvilStateName::FULL, simTime());
				}
			} else if(appmsg->getReqResKind() == ReqResKind::COMMAND) {
				if (p < this->parent->commandRequestBlockProb) { // Block
					this->parent->emit(this->parent->commandRequestBlockSignal, true);
					msg->setAtkStatus(MITMKind::BLOCK);
					if(parent->isLogging) parent->logger->log(msg, EvilStateName::FULL, simTime());
					delete pckt;
					delete msg;
					return;
				} else if (p - this->parent->commandRequestBlockProb < this->parent->commandRequestCompromisedProb) { // Compromise
					this->parent->emit(this->parent->commandRequestCompromisedSignal, true);
					msg->setAtkStatus(MITMKind::COMPR);
					msg->setData(9);
					if(parent->isLogging) parent->logger->log(msg, EvilStateName::FULL, simTime());
				} else {
					if(parent->isLogging) parent->logger->log(msg, EvilStateName::FULL, simTime());
				}
			}
		}

		enqueueNSchedule(msg);

    }

    delete pckt;
}

void FromSerOpListener::enqueueNSchedule(MmsMessage* msg) {
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
