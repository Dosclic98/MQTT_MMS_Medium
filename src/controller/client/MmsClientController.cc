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

#define SEND_MMS_CONNECT 5
#define SEND_MMS_READ 6
#define SEND_MMS_COMMAND 7
#define SEND_MMS_DISCONNECT 8

Define_Module(inet::MmsClientController);

using namespace inet;

MmsClientController::MmsClientController() {
	// TODO Auto-generated constructor stub
}

MmsClientController::~MmsClientController() {
	// TODO Auto-generated destructor stub
}

void MmsClientController::initialize() {
	cmdPubSig = registerSignal("cliCmdSig");

	resListener = new MmsResListener(this);
	msgListener = new MmsMsgListener(this);
	// Subscribe listeners on the right module and signal
	getParentModule()->getParentModule()->subscribe("cliResSig", resListener);
	getParentModule()->getParentModule()->subscribe("cliMsgSig", msgListener);

    // Schedule the register for measure MMS message
	simtime_t dMeas = simTime() + SimTime(2, SIMTIME_S);
	cMessage* tmpMeas = new cMessage("SENDMEAS", SEND_MMS_CONNECT);
	scheduleAt(dMeas, tmpMeas);

    // Schedule a Read send
    simtime_t dRead = simTime() + SimTime(par("sendReadInterval").intValue(), SIMTIME_S);
    cMessage* tmpRead = new cMessage("SENDREAD", SEND_MMS_READ);
    scheduleAt(dRead, tmpRead);
    // // Schedule a Command send
    simtime_t dCommand = simTime() + SimTime(par("sendCommandInterval").intValue(), SIMTIME_S);
    cMessage* tmpCommand = new cMessage("SENDCOMMAND", SEND_MMS_COMMAND);
    scheduleAt(dCommand, tmpCommand);
}

// TODO Just a temporary solution to test the client controller <--> operator communication
void MmsClientController::handleMessage(cMessage* msg) {
	if(msg->isSelfMessage()) {
		switch(msg->getKind()) {
			case SEND_MMS_CONNECT: {
				SendMmsConnect* cliOp = new SendMmsConnect(idCounter);
				this->propagate(cliOp);
				break;
			}
			case SEND_MMS_READ: {
				SendMmsRequest* cliOp = new SendMmsRequest(idCounter, ReqResKind::READ, 0);
				this->propagate(cliOp);
				break;
			}
			case SEND_MMS_COMMAND: {
				SendMmsRequest* cliOp = new SendMmsRequest(idCounter, ReqResKind::COMMAND, 0);
				this->propagate(cliOp);
				break;
			}
			case SEND_MMS_DISCONNECT: {
				SendMmsDisconnect* cliOp = new SendMmsDisconnect(idCounter);
				this->propagate(cliOp);
				break;
			}
			default: {
				// Should not be here
			}
		}
		idCounter++;
		delete msg;
	}
}

void MmsClientController::propagate(IOperation* op) {
	emit(this->cmdPubSig, op);
}


void MmsClientController::next(Packet* msg) {

}

void MmsClientController::evalRes(IResult* res) {

}

