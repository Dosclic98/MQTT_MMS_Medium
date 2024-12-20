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

#include "MmsClientController.h"

#include "../fsm/concrete/OpFSM.h"
#include "../fsm/factory/concrete/MmsClientFSMFactory.h"
#include "../listener/MsgListener.h"
#include "../listener/ResListener.h"

using namespace inet;

Define_Module(MmsClientController);

MmsClientController::MmsClientController() {
	// TODO Auto-generated constructor stub
}

MmsClientController::~MmsClientController() {

}

void MmsClientController::finish() {
	delete this->fsmFactory;
	delete this->controlFSM;
}

void MmsClientController::initialize() {
	ControllerBinder* binder = getBinder();
	binder->registerController(this);

    char strResSubSig[30];
    char strMsgSubSig[30];
    char strCliCmdPubSig[30];
    sprintf(strResSubSig, "cliResSig-%d", this->getIndex());
    sprintf(strMsgSubSig, "cliMsgSig-%d", this->getIndex());
    sprintf(strCliCmdPubSig, "cliCmdSig-%d", this->getIndex());
	cmdPubSig = registerSignal(strCliCmdPubSig);

	resListener = new ResListener(this);
	msgListener = new MsgListener(this);
	// Subscribe listeners on the right module and signal
	getParentModule()->getParentModule()->subscribe(strResSubSig, resListener);
	getParentModule()->getParentModule()->subscribe(strMsgSubSig, msgListener);

    this->fsmFactory = new MmsClientFSMFactory(this, this->getIndex());
    this->controlFSM = this->fsmFactory->build();
}

void MmsClientController::scheduleEvent(cMessage* event, simtime_t delay) {
	Enter_Method("Possible scheduling by Attack graph");
	take(event);
	cancelEvent(event);
    // Schedule event after delay from now
	simtime_t sTime = simTime() + delay;
	scheduleAt(sTime, event);
}

void MmsClientController::descheduleEvent(cMessage* event) {
	Enter_Method("Possible descheduling by Attack graph");
	cancelEvent(event);
}

// TODO Just a temporary solution to test the client controller <--> operator communication
void MmsClientController::handleMessage(cMessage* msg) {
	if(msg->isSelfMessage()) {
		this->controlFSM->next(msg);
	}
}

void MmsClientController::propagate(IOperation* op) {
	emit(this->cmdPubSig, op);
}


void MmsClientController::next(Packet* msg) {
	// At the moment the controller does not manage MMS responses back
}

void MmsClientController::evalRes(IResult* res) {

}

void MmsClientController::enqueueNSchedule(IOperation* operation) {

}

