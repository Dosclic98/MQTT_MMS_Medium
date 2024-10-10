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

#include "HttpServerController.h"

#include "../fsm/factory/concrete/HttpServerHistorianFSMFactory.h"
#include "../listener/MsgListener.h"
#include "../listener/ResListener.h"

using namespace inet;

Define_Module(HttpServerController);

void HttpServerController::initialize() {
    ControllerBinder* binder = getBinder();
    binder->registerController(this);
    EV << "Server Controller Pathname: " << binder->getPathName(getId()) << "\n";

    char strCmdPubSig[30] = "httpSerCmdSig";
    char strSerResSig[30] = "httpSerResSig";
    char strSerMsgSig[30] = "httpSerMsgSig";

    cmdPubSig = registerSignal(strCmdPubSig);

    resListener = new ResListener(this);
    msgListener = new MsgListener(this);
    // Subscribe listeners on the right module and signal
    getParentModule()->subscribe(strSerResSig, resListener);
    getParentModule()->subscribe(strSerMsgSig, msgListener);

    controllerStatus = false;

    std::string cfsmType = par("cfsmType").str();
    if(cfsmType == std::string("historian")) {
        this->fsmFactory = new HttpServerHistorianFSMFactory(this);
    } else if(cfsmType == std::string("station")) {
        // TODO implement station computer FSM factory
    }

    this->controlFSM = this->fsmFactory->build();
}

void HttpServerController::scheduleEvent(cMessage* event, simtime_t delay) {
    cancelEvent(event);
    // Schedule event after delay from now
    simtime_t sTime = simTime() + delay;
    scheduleAt(sTime, event);
}

void HttpServerController::descheduleEvent(cMessage* event) {
    cancelEvent(event);
}

void HttpServerController::handleMessage(cMessage *msg) {
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

void HttpServerController::enqueueNSchedule(IOperation* operation) {
    if(!controllerStatus) {
        controllerStatus = true;
        operationQueue.insert(operation);
        scheduleAt(simTime() + SimTime(par("serviceTime").intValue(), SIMTIME_MS), departureEvent);
    }
    else operationQueue.insert(operation);
}

void HttpServerController::propagate(IOperation* op) {
    emit(this->cmdPubSig, op);
}

void HttpServerController::next(Packet* msg) {
    this->controlFSM->next(msg);
}

void HttpServerController::evalRes(IResult* res) {
    // Does nothing at the moment
}

HttpServerController::HttpServerController() {
    // TODO Auto-generated constructor stub
}

HttpServerController::~HttpServerController() {
    cancelAndDelete(departureEvent);
}
