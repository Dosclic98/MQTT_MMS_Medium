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
#include "../../operation/client/concrete/SendMmsConnect.h"
#include "../../operation/client/concrete/SendMmsDisconnect.h"
#include "../../operation/client/concrete/SendMmsRequest.h"

using namespace inet;

Define_Module(MmsClientController);

MmsClientController::MmsClientController() {
	// TODO Auto-generated constructor stub
}

MmsClientController::~MmsClientController() {
	// TODO Auto-generated destructor stub
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

    // Schedule the register for measure MMS message
	simtime_t dMeas = simTime() + SimTime(2, SIMTIME_S);
	cMessage* tmpMeas = new cMessage("SENDMEAS", SEND_MMS_CONNECT);
	scheduleAt(dMeas, tmpMeas);

    // Schedule a Read send
    simtime_t dRead = simTime() + SimTime(par("sendReadInterval").intValue(), SIMTIME_S);
    cMessage* tmpRead = new cMessage("SENDREAD", SEND_MMS_READ);
    scheduleAt(dRead, tmpRead);
    // Schedule a Command send
    simtime_t dCommand = simTime() + SimTime(par("sendCommandInterval").intValue(), SIMTIME_S);
    cMessage* tmpCommand = new cMessage("SENDCOMMAND", SEND_MMS_COMMAND);
    scheduleAt(dCommand, tmpCommand);

    sendMmsConnectFactory = new SendMmsConnectFactory(this);
    sendMmsDisconnectFactory = new SendMmsDisconnectFactory(this);
    sendMmsRequestFactory = new SendMmsRequestFactory(this);
}

// TODO Just a temporary solution to test the client controller <--> operator communication
void MmsClientController::handleMessage(cMessage* msg) {
	if(msg->isSelfMessage()) {
		switch(msg->getKind()) {
			case SEND_MMS_CONNECT: {
				sendMmsConnectFactory->build(msg);
				break;
			}
			case SEND_MMS_READ: {
				sendMmsRequestFactory->build(msg);
				break;
			}
			case SEND_MMS_COMMAND: {
				sendMmsRequestFactory->build(msg);
				break;
			}
			case SEND_MMS_DISCONNECT: {
				sendMmsDisconnectFactory->build(msg);
				break;
			}
			default: {
				// Should not be here
			}
		}
		delete msg;
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

