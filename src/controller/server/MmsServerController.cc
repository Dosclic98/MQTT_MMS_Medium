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
#include "inet/common/socket/SocketTag_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/packet/Message.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/TimeTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/tcp/TcpCommand_m.h"

#include "../fsm/operation/OpFSM.h"
#include "../fsm/factory/concrete/MmsServerFSMFactory.h"
#include "../listener/MsgListener.h"
#include "../listener/ResListener.h"

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

	char strCmdPubSig[30];
	char strSerResSig[30];
	char strSerMsgSig[30];
	sprintf(strCmdPubSig, "serCmdSig-%d", getParentModule()->getIndex());
	sprintf(strSerResSig, "serResSig-%d", getParentModule()->getIndex());
	sprintf(strSerMsgSig, "serMsgSig-%d", getParentModule()->getIndex());
	cmdPubSig = registerSignal(strCmdPubSig);

	resListener = new ResListener(this);
	msgListener = new MsgListener(this);
	// Subscribe listeners on the right module and signal
	getParentModule()->getParentModule()->subscribe(strSerResSig, resListener);
	getParentModule()->getParentModule()->subscribe(strSerMsgSig, msgListener);

    departureEvent = new cMessage("Server Departure");
	sendDataEvent = new cMessage("Register Data To Send Event");
	controllerStatus = false;

    this->fsmFactory = new MmsServerFSMFactory(this);
    this->controlFSM = this->fsmFactory->build();
}

void MmsServerController::scheduleEvent(cMessage* event, simtime_t delay) {
	cancelEvent(event);
    // Schedule event after delay from now
	simtime_t sTime = simTime() + delay;
	scheduleAt(sTime, event);
}

void MmsServerController::descheduleEvent(cMessage* event) {
	cancelEvent(event);
}

void MmsServerController::handleMessage(cMessage *msg) {
    if(msg == sendDataEvent) {
    	this->controlFSM->next(msg);
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

cMessage* MmsServerController::getSendMeasuresEvent() {
	return sendDataEvent;
}

void MmsServerController::enqueueNSchedule(IOperation* operation) {
	if(!controllerStatus) {
		controllerStatus = true;
		operationQueue.insert(operation);
		scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS), departureEvent);
	}
	else operationQueue.insert(operation);
}

void MmsServerController::insertMmsSubscriber(int connId, int evilServerConnId) {
	clientConnIdList.push_back({connId, evilServerConnId});
}

void MmsServerController::removeMmsSubscriber(int connId) {
    clientConnIdList.remove_if([&](std::pair<int, int>& p) {
        return p.first == connId;
    });
}

std::list<std::pair<int,int>>& MmsServerController::getMmsSubscriberList() {
	return clientConnIdList;
}

void MmsServerController::propagate(IOperation* op) {
	emit(this->cmdPubSig, op);
}

void MmsServerController::next(Packet* msg) {
	this->controlFSM->next(msg);
}

void MmsServerController::evalRes(IResult* res) {

}

