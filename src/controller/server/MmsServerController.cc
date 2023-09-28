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

#include "MmsServerController.h"
#include "../listener/MmsMsgListener.h"
#include "../listener/MmsResListener.h"
#include "../../operation/server/concrete/ForwardDeparture.h"
#include "inet/common/socket/SocketTag_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/packet/Message.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/TimeTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/tcp/TcpCommand_m.h"

#include "../../operation/factory/event/concrete/GenerateMeasuresFactory.h"
#include "../../operation/factory/packet/concrete/ForwardDepartureFactory.h"


using namespace inet;

Define_Module(MmsServerController);

MmsServerController::MmsServerController() {
	// TODO Auto-generated constructor stub
}

MmsServerController::~MmsServerController() {
	cancelAndDelete(departureEvent);
	cancelAndDelete(sendDataEvent);
	// TODO Empty the queue
}

void MmsServerController::initialize() {
	ControllerBinder* binder = getBinder();
	binder->registerController(this);
	EV << "Server Controller Pathname: " << binder->getPathName(getId()) << "\n";

	forwardDepartureFactory = new ForwardDepartureFactory(this);
	generateMeasuresFactory = new GenerateMeasuresFactory(this);

	char strCmdPubSig[30];
	char strSerResSig[30];
	char strSerMsgSig[30];
	sprintf(strCmdPubSig, "serCmdSig-%d", getParentModule()->getIndex());
	sprintf(strSerResSig, "serResSig-%d", getParentModule()->getIndex());
	sprintf(strSerMsgSig, "serMsgSig-%d", getParentModule()->getIndex());
	cmdPubSig = registerSignal(strCmdPubSig);

	resListener = new MmsResListener(this);
	msgListener = new MmsMsgListener(this);
	// Subscribe listeners on the right module and signal
	getParentModule()->getParentModule()->subscribe(strSerResSig, resListener);
	getParentModule()->getParentModule()->subscribe(strSerMsgSig, msgListener);

    departureEvent = new cMessage("Server Departure");
	sendDataEvent = new cMessage("Register Data To Send Event");
	controllerStatus = false;
	scheduleAt(1, sendDataEvent);
}

void MmsServerController::handleMessage(cMessage *msg) {
    if(msg == sendDataEvent) {
    	generateMeasuresFactory->build(sendDataEvent);
    }
    if(msg == departureEvent) {
    	if(!operationQueue.isEmpty()) {
        	IOperation* opDep = check_and_cast<IOperation*>(operationQueue.pop());
        	propagate(opDep);
        	if(!operationQueue.isEmpty()) {
        		scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS), departureEvent);
        	} else controllerStatus = false;
    	}
    }
}

void MmsServerController::enqueueNSchedule(IOperation* operation) {
	if(!controllerStatus) {
		controllerStatus = true;
		operationQueue.insert(operation);
		scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS), departureEvent);
	}
	else operationQueue.insert(operation);
}

void MmsServerController::propagate(IOperation* op) {
	emit(this->cmdPubSig, op);
}

void MmsServerController::next(Packet* msg) {
	forwardDepartureFactory->build(msg);
}

void MmsServerController::evalRes(IResult* res) {

}

