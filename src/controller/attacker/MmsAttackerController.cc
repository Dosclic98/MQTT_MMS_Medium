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
#include "../fsm/factory/concrete/MmsAttackerFSMFactory.h"

using namespace inet;

Define_Module(MmsAttackerController);

void MmsAttackerController::initialize() {
	ControllerBinder* binder = getBinder();
	binder->registerController(this);

	connect = new cMessage("TCPCONNECT", MSGKIND_CONNECT);

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

	this->fsmFactory = new MmsAttackerFSMFactory(this);
	this->controlFSM = this->fsmFactory->build();

	this->scheduleNextTcpConnect();
}

void MmsAttackerController::scheduleNextTcpConnect() {
	cancelEvent(this->connect);
    // Schedule TCP connection (1 second after initialization)
	simtime_t dConnect = simTime() + SimTime(1, SIMTIME_S);
	scheduleAt(dConnect, this->connect);
}

void MmsAttackerController::handleMessage(cMessage *msg) {
	switch (msg->getKind()) {
		case MSGKIND_CONNECT:
			this->controlFSM->next(msg);
		case MSGKIND_SEND:
			if(!operationQueue.isEmpty()) {
				IOperation* op = check_and_cast<IOperation*>(operationQueue.pop());
				propagate(op);
				if(!operationQueue.isEmpty()) {
					simtime_t d = simTime() + SimTime(round(par("thinkTime").doubleValue()), SIMTIME_MS);
					timeoutMsg->setKind(MSGKIND_SEND);
					scheduleAt(d, timeoutMsg);
				} else { controllerStatus = false; }
			} else { controllerStatus = false; }
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

	this->controlFSM->next(pckt);

    delete pckt;
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

void MmsAttackerController::scheduleEvent(cMessage* event, simtime_t delay) {
	Enter_Method("Possible scheduling by Attack graph");
	take(event);
	cancelEvent(event);
    // Schedule event after delay from now
	simtime_t sTime = simTime() + delay;
	scheduleAt(sTime, event);
}

void MmsAttackerController::descheduleEvent(cMessage* event) {
	Enter_Method("Possible descheduling by Attack graph");
	cancelEvent(event);
}

bool MmsAttackerController::isAtkLogging() {
	Enter_Method("Possible call by Attack graph");
	return this->isLogging;
}

void MmsAttackerController::log(const MmsMessage* msg, const char* evilStateName, simtime_t timestamp) {
	logger->log(msg, evilStateName, timestamp);
}

int MmsAttackerController::getNumGenReq() {
	return numGenReq;
}

int MmsAttackerController::getFakeGenReqThresh() {
	return fakeGenReqThresh;
}

void MmsAttackerController::setNumGenReq(int numGenReq) {
	this->numGenReq = numGenReq;
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
	cancelAndDelete(this->connect);
	operationQueue.clear();
	delete this->fsmFactory;
	delete this->controlFSM;
}
