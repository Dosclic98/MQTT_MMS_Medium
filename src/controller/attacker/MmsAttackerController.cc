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
#include "../../operation/attacker/concrete/ForwardMmsMessageToServer.h"

#include "../../operation/factory/packet/concrete/ForwardMmsMessageToServerFactory.h"
#include "../../operation/factory/packet/concrete/ForwardMmsMessageToClientFactory.h"
#include "../../operation/factory/packet/concrete/ForwardFakeMmsMessageToServerFactory.h"

using namespace inet;

Define_Module(MmsAttackerController);

void MmsAttackerController::initialize() {
	ControllerBinder* binder = getBinder();
	binder->registerController(this);

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

    // Initializing message factories
    forwardMmsMessageToClientFactory = new ForwardMmsMessageToClientFactory(this);
    forwardMmsMessageToServerFactory = new ForwardMmsMessageToServerFactory(this);
    forwardFakeMmsMessageToServerFactory = new ForwardFakeMmsMessageToServerFactory(this);

	fakeGenReqThresh = par("fakeGenReqThresh").intValue();
	numGenReq = 0;

    controllerStatus = false;
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
			if(!operationQueue.isEmpty()) {
				IOperation* op = check_and_cast<IOperation*>(operationQueue.pop());
				propagate(op);
				controllerStatus = false;
			}
			break;

		default:
			throw cRuntimeError("Invalid timer msg: kind=%d", msg->getKind());
			break;
	}
}

void MmsAttackerController::next(Packet* pckt) {
	// This is a Socket connection established message
	if(pckt == nullptr) {
		isSocketConnected = true;
	    // Forward the packets waiting to be forwarded to the server
	    if(!operationQueue.isEmpty() && !controllerStatus) {
	    	simtime_t d = simTime() + SimTime(round(par("thinkTime").doubleValue()), SIMTIME_MS);
	    	timeoutMsg->setKind(MSGKIND_SEND);
	    	scheduleAt(d, timeoutMsg);
			controllerStatus = true;
	    }
		return;
	}

	// We must see if the message is directed to the client or to the server
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

				forwardFakeMmsMessageToServerFactory->build(pckt);
			}
		}

		bubble("Sent to internal client!");
		EV_INFO << "Conn ID:" << appmsg->getEvilServerConnId() << "\n";

		MMSKind messageKind = appmsg->getMessageKind();

	    if(messageKind == MMSKind::GENRESP || messageKind == MMSKind::MEASURE) {
	    	forwardMmsMessageToClientFactory->build(pckt);
	    } else {
	    	forwardMmsMessageToServerFactory->build(pckt);
	    }

	    delete pckt;
    }
}

void MmsAttackerController::enqueueNSchedule(IOperation* operation) {
	if(!controllerStatus && isSocketConnected) {
		operationQueue.insert(operation);
		simtime_t d = simTime() + SimTime(round(par("thinkTime").doubleValue()), SIMTIME_MS);
		// The client is already connected to the server, so when the data arrives we schedule to send it (MSG_KIND_SEND)
		timeoutMsg->setKind(MSGKIND_SEND);
		scheduleAt(d, timeoutMsg);
		controllerStatus = true;
	} else {
		operationQueue.insert(operation);
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
	cancelAndDelete(timeoutMsg);
	operationQueue.clear();
}
