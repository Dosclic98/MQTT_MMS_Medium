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

#include "MmsAttackerController.h"
#include "../listener/MmsMsgListener.h"
#include "../listener/MmsResListener.h"
#include "../../operation/attacker/concrete/ForwardMmsMessageToClient.h"

using namespace inet;

Define_Module(MmsAttackerController);

void MmsAttackerController::initialize() {
	char strCmdPubSig[30];
	sprintf(strCmdPubSig, "atkCmdSig-%d", this->getIndex());
	cmdPubSig = registerSignal(strCmdPubSig);

    char strResSubSig[30];
    char strMsgSubSig[30];
    sprintf(strResSubSig, "atkResSig-%d", this->getIndex());
    sprintf(strMsgSubSig, "atkMsgSig-%d", this->getIndex());
	resListener = new MmsResListener(this);
	msgListener = new MmsMsgListener(this);
	// Subscribe listeners on the right module and signal
	getParentModule()->getParentModule()->subscribe(strResSubSig, resListener);
	getParentModule()->getParentModule()->subscribe(strMsgSubSig, msgListener);

	messageCopier = new MmsMessageCopier();

	cEnvir* ev = getSimulation()->getActiveEnvir();
	isLogging = par("isLogging");
    if(isLogging) {
    	logger = new EvilLogger(ev->getConfigEx()->getActiveRunNumber(), getIndex());
    }

	fakeGenReqThresh = par("fakeGenReqThresh").intValue();
	numGenReq = 0;

    previousResponseSent = true;
    isSocketConnected = false;

    sendMsgEvent = new cMessage("Send message event");
    timeoutMsg = new cMessage("timer");

    genericFakeReqResSignal = registerSignal("genericFakeReqResSignal");
    readRequestBlockSignal = registerSignal("readRequestBlockSignal");
    readRequestCompromisedSignal = registerSignal("readRequestCompromisedSignal");
    commandRequestBlockSignal = registerSignal("commandRequestBlockSignal");
    commandRequestCompromisedSignal = registerSignal("commandRequestCompromisedSignal");
    measureBlockSignal = registerSignal("measureBlockSignal");
    measureCompromisedSignal = registerSignal("measureCompromisedSignal");
    readResponseBlockSignal = registerSignal("readResponseBlockSignal");
    readResponseCompromisedSignal = registerSignal("readResponseCompromisedSignal");
    commandResponseBlockSignal = registerSignal("commandResponseBlockSignal");
    commandResponseCompromisedSignal = registerSignal("commandResponseCompromisedSignal");

	readResponseBlockProb = par("readResponseBlockProb").doubleValue();
	readResponseCompromisedProb = par("readResponseCompromisedProb").doubleValue();
	commandResponseBlockProb = par("commandResponseBlockProb").doubleValue();
	commandResponseCompromisedProb = par("commandResponseCompromisedProb").doubleValue();

	readRequestBlockProb = par("readRequestBlockProb").doubleValue();
	readRequestCompromisedProb = par("readRequestCompromisedProb").doubleValue();
	commandRequestBlockProb = par("commandRequestBlockProb").doubleValue();
	commandRequestCompromisedProb = par("commandRequestCompromisedProb").doubleValue();

	measureBlockProb = par("measureBlockProb").doubleValue();
	measureCompromisedProb = par("measureCompromisedProb").doubleValue();

	if(measureBlockProb < 0 || measureCompromisedProb < 0 || (measureBlockProb + measureCompromisedProb) > 1) {
		throw std::invalid_argument("Invalid measure block/compromise probability");
	}
	if(readRequestBlockProb < 0 || readRequestCompromisedProb < 0 || (readRequestBlockProb + readRequestCompromisedProb) > 1) {
		throw std::invalid_argument("Invalid read request block/compromise probability");
	}
	if(commandRequestBlockProb < 0 || commandRequestCompromisedProb < 0 || (commandRequestBlockProb + commandRequestCompromisedProb) > 1) {
		throw std::invalid_argument("Invalid command request block/compromise probability");
	}
	if(readResponseBlockProb < 0 || readResponseCompromisedProb < 0 || (readResponseBlockProb + readResponseCompromisedProb) > 1) {
		throw std::invalid_argument("Invalid read response block/compromise probability");
	}
	if(commandResponseBlockProb < 0 || commandResponseCompromisedProb < 0 || (commandResponseBlockProb + commandResponseCompromisedProb) > 1) {
		throw std::invalid_argument("Invalid command response block/compromise probability");
	}

}

void MmsAttackerController::handleMessage(cMessage *msg) {
	switch (msg->getKind()) {
		case MSGKIND_SEND:
			if(!msgQueue.isEmpty()) {
				MmsMessage* msgPopped = check_and_cast<MmsMessage*>(msgQueue.pop());
				const auto& payload = messageCopier->copyMessage(msgPopped, true);
				Packet *packet = new Packet("data");
				packet->insertAtBack(payload);
				ForwardMmsMessageToClient* msgToCli = new ForwardMmsMessageToClient(idCounter, packet);
				propagate(msgToCli);
				//delete msgPopped;
			}
			break;

		//default:
			//throw cRuntimeError("Invalid timer msg: kind=%d", msg->getKind());
			//break;
	}
}

void MmsAttackerController::next(Packet* pckt) {
	// This is a Socket connection established message
	if(pckt == nullptr) {
		isSocketConnected = true;
	    // Forward the packets waiting to be forwarded to the server
	    if(!msgQueue.isEmpty() && previousResponseSent) {
	    	simtime_t d = simTime() + SimTime(round(par("thinkTime").doubleValue()), SIMTIME_MS);
	    	timeoutMsg->setKind(MSGKIND_SEND);
	    	scheduleAt(d, timeoutMsg);
			previousResponseSent = false;
	    }
		return;
	}

	// We must see if the message is directed to the clisnt or to the server
    auto chunk = pckt->peekDataAt(B(0), pckt->getTotalLength());
    queue.push(chunk);
    while (queue.has<MmsMessage>(b(-1))) {
    	const auto& appmsg = queue.pop<MmsMessage>(b(-1));
    	if(appmsg->getMessageKind() == MMSKind::GENRESP && appmsg->getEvilServerConnId() == -1) {
            // Emit signal for generic fake Req Res
    		emit(genericFakeReqResSignal, true);
    		if(isLogging) logger->log(appmsg.get(), EvilStateName::FULL, simTime());
            delete pckt;
            return;
    	}
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
				if(isLogging) logger->log(msg, EvilStateName::FULL, simTime());

				enqueueNSchedule(msg);
			}
		}

		const auto& msg = messageCopier->copyMessage(appmsg.get(), appmsg->getEvilServerConnId(), true);
		Packet *packet = new Packet("data");

		bubble("Sent to internal client!");
		EV_INFO << "Conn ID:" << msg->getEvilServerConnId() << "\n";

		MMSKind messageKind = appmsg->getMessageKind();
		ReqResKind reqResKind = appmsg->getReqResKind();

	    double p = this->uniform(0.0, 1.0);
	    if (messageKind == MMSKind::MEASURE) {
	        if (p < measureBlockProb) { //Block
	        	bubble("Measure blocked");
	            emit(measureBlockSignal, true);
	            msg->setAtkStatus(MITMKind::BLOCK);
	            if(isLogging) logger->log(msg.get(), EvilStateName::FULL, simTime());
	            delete packet;
	            delete pckt;
	            return;
	        } else if (p - measureBlockProb < measureCompromisedProb) { //Compromise
	        	bubble("Measure compromised");
	            emit(measureCompromisedSignal, true);
	            msg->setAtkStatus(MITMKind::COMPR);
	            msg->setData(9);
	            if(isLogging) logger->log(msg.get(), EvilStateName::FULL, simTime());
	        } else {
	        	bubble("Measure arrived from server");
	        	if(isLogging) logger->log(msg.get(), EvilStateName::FULL, simTime());
	        }
	    } else if (messageKind == MMSKind::GENRESP) {
	    	if(reqResKind == ReqResKind::READ) {
		        if (p < readResponseBlockProb) { // Block
		        	bubble("Read response blocked");
		            emit(readResponseBlockSignal, true);
		            msg->setAtkStatus(MITMKind::BLOCK);
		            if(isLogging) logger->log(msg.get(), EvilStateName::FULL, simTime());
		            delete packet;
		            delete pckt;
		            return;
		        } else if (p - readResponseBlockProb < readResponseCompromisedProb) { // Compromise
		        	bubble("Read response compromised");
		            emit(readResponseCompromisedSignal, true);
		            msg->setAtkStatus(MITMKind::COMPR);
		            msg->setData(9);
		            if(isLogging) logger->log(msg.get(), EvilStateName::FULL, simTime());
		        } else {
		        	bubble("Read response arrived from server");
		        	if(isLogging) logger->log(msg.get(), EvilStateName::FULL, simTime());
		        }
	    	} else if(reqResKind == ReqResKind::COMMAND) {
		        if (p < commandResponseBlockProb) { // Block
		        	bubble("Command response blocked");
		            emit(commandResponseBlockSignal, true);
		            msg->setAtkStatus(MITMKind::BLOCK);
		            if(isLogging) logger->log(msg.get(), EvilStateName::FULL, simTime());
		            delete packet;
		            delete pckt;
		            return;
		        } else if (p - commandResponseBlockProb < commandResponseCompromisedProb) { // Compromise
		        	bubble("Command response compromised");
		            emit(commandResponseCompromisedSignal, true);
		            msg->setAtkStatus(MITMKind::COMPR);
		            msg->setData(9);
		            if(isLogging) logger->log(msg.get(), EvilStateName::FULL, simTime());
		        } else {
		        	bubble("Command response arrived from server");
		        	if(isLogging) logger->log(msg.get(), EvilStateName::FULL, simTime());
		        }
	    	}
	    }else if(appmsg->getMessageKind() == MMSKind::GENREQ) {
			if(appmsg->getReqResKind() == ReqResKind::READ) {
				if (p < readRequestBlockProb) { // Block
					emit(readRequestBlockSignal, true);
					msg->setAtkStatus(MITMKind::BLOCK);
					if(isLogging) logger->log(msg.get(), EvilStateName::FULL, simTime());
					delete pckt;
					delete msg.get();
					return;
				} else if (p - readRequestBlockProb < readRequestCompromisedProb) { // Compromise
					msg->setAtkStatus(MITMKind::COMPR);
					msg->setData(9);
					if(isLogging) logger->log(msg.get(), EvilStateName::FULL, simTime());
					emit(readRequestCompromisedSignal, true);
				} else {
					if(isLogging) logger->log(msg.get(), EvilStateName::FULL, simTime());
				}
			} else if(appmsg->getReqResKind() == ReqResKind::COMMAND) {
				if (p < commandRequestBlockProb) { // Block
					emit(commandRequestBlockSignal, true);
					msg->setAtkStatus(MITMKind::BLOCK);
					if(isLogging) logger->log(msg.get(), EvilStateName::FULL, simTime());
					delete pckt;
					delete msg.get();
					return;
				} else if (p - commandRequestBlockProb < commandRequestCompromisedProb) { // Compromise
					emit(commandRequestCompromisedSignal, true);
					msg->setAtkStatus(MITMKind::COMPR);
					msg->setData(9);
					if(isLogging) logger->log(msg.get(), EvilStateName::FULL, simTime());
				} else {
					if(isLogging) logger->log(msg.get(), EvilStateName::FULL, simTime());
				}
			}
		}



	    if(messageKind == MMSKind::GENRESP || messageKind == MMSKind::MEASURE) {
	    	packet->insertAtBack(msg);
	    	packet->addTag<SocketInd>()->setSocketId(appmsg->getEvilServerConnId());

	    	ForwardMmsMessageToClient* cliOp = new ForwardMmsMessageToClient(idCounter, packet);
		    this->propagate(cliOp);
		    idCounter++;
	    } else {
	    	enqueueNSchedule(msg.get());
	    	// Just used to forward messages directed to the clients
	    	//delete packet;
	    }

	    //delete pckt;
    }
}

void MmsAttackerController::enqueueNSchedule(MmsMessage* msg) {
	if(previousResponseSent && isSocketConnected) {
		msgQueue.insert(msg);
		simtime_t d = simTime() + SimTime(round(par("thinkTime").doubleValue()), SIMTIME_MS);
		// The client is already connected to the server, so when the data arrives we schedule to send it (MSG_KIND_SEND)
		timeoutMsg->setKind(MSGKIND_SEND);
		scheduleAt(d, timeoutMsg);
		previousResponseSent = false;
	} else {
		msgQueue.insert(msg);
	}
}

void MmsAttackerController::propagate(IOperation* op) {
	emit(this->cmdPubSig, op);
}

void MmsAttackerController::evalRes(IResult* res) {

}

MmsAttackerController::~MmsAttackerController() {
	if(isLogging) delete logger;
	cancelAndDelete(sendMsgEvent);
	msgQueue.clear();
}
