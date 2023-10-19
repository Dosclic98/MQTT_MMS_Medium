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
#include "../listener/MmsMsgListener.h"
#include "../listener/MmsResListener.h"
#include "../fsm/operation/OpFSM.h"
#include "../fsm/factory/concrete/MmsClientFSMFactory.h"

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
	cancelAndDelete(this->connect);
	cancelAndDelete(this->meas);
	cancelAndDelete(this->read);
	cancelAndDelete(this->command);
	cancelAndDelete(this->disconnect);
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

	resListener = new MmsResListener(this);
	msgListener = new MmsMsgListener(this);
	// Subscribe listeners on the right module and signal
	getParentModule()->getParentModule()->subscribe(strResSubSig, resListener);
	getParentModule()->getParentModule()->subscribe(strMsgSubSig, msgListener);

	connect = new cMessage("TCPCONNECT", SEND_TCP_CONNECT);
	meas = new cMessage("SENDMEAS", SEND_MMS_CONNECT);
	read = new cMessage("SENDREAD", SEND_MMS_READ);
	command = new cMessage("SENDCOMMAND", SEND_MMS_COMMAND);
	disconnect = new cMessage("SENDDISCONNECT", SEND_MMS_DISCONNECT);

	scheduleNextTcpConnect();
	scheduleNextMmsRead();
	scheduleNextMmsCommand();

    this->fsmFactory = new MmsClientFSMFactory(this, this->getIndex());
    this->controlFSM = this->fsmFactory->build();
}

void MmsClientController::scheduleNextTcpConnect() {
    // Schedule TCP connection (1 second after initialization)
	simtime_t dConnect = simTime() + SimTime(2, SIMTIME_S);
	scheduleAt(dConnect, this->connect);
}

void MmsClientController::scheduleNextMmsConnect() {
    // Schedule the register for measure MMS message (1 second after TCP connection)
	simtime_t dMeas = simTime() + SimTime(1, SIMTIME_S);
	scheduleAt(dMeas, this->meas);
}

void MmsClientController::scheduleNextMmsRead() {
    // Schedule a Read send
    simtime_t dRead = simTime() + SimTime(par("sendReadInterval").intValue(), SIMTIME_S);
    scheduleAt(dRead, this->read);
}

void MmsClientController::scheduleNextMmsCommand() {
    // Schedule a Command send
    simtime_t dCommand = simTime() + SimTime(par("sendCommandInterval").intValue(), SIMTIME_S);
    scheduleAt(dCommand, this->command);
}

void MmsClientController::scheduleNextMmsDisconnect() {
	// Schedule MMS (and also TCP) disconnect
	simtime_t dDisconnect = simTime() + SimTime(30, SIMTIME_S);
	scheduleAt(dDisconnect, this->disconnect);
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

