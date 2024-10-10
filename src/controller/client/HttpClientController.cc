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

#include "HttpClientController.h"
#include "../listener/MsgListener.h"
#include "../listener/ResListener.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "../fsm/factory/concrete/HttpClientFSMFactory.h"

using namespace inet;

#define MSGKIND_CONNECT    1
#define MSGKIND_SEND       2
#define MSGKIND_CLOSE      3

Define_Module(HttpClientController);

void HttpClientController::initialize() {
    ControllerBinder* binder = getBinder();
    binder->registerController(this);

    // Initialize operation, result and packet signals
    const char* strCmdPubSig = "httpCmdSig";
    cmdPubSig = registerSignal(strCmdPubSig);

    const char* strResSubSig = "httpResSig";
    const char* strMsgSubSig = "httpMsgSig";
    resListener = new ResListener(this);
    msgListener = new MsgListener(this);
    // Subscribe listeners on the right module and signal
    getParentModule()->subscribe(strResSubSig, resListener);
    getParentModule()->subscribe(strMsgSubSig, msgListener);

    connectTimeout = par("connectTimeout");

    controllerStatus = false;

    // HTTP client FSM factory
    this->fsmFactory = new HttpClientFSMFactory(this);
    this->controlFSM = this->fsmFactory->build();
}

void HttpClientController::handleMessage(cMessage *msg) {
    if(msg == connectionTimer || msg == timeoutTimer ||
            msg == disconnectionTimer || msg == sendRequestTimer ||
            msg == startingTimer) {
        this->controlFSM->next(msg);
    } else {
        EV_INFO << "LOLLOSO\n";
        if(msg == thinkTimer) {
            if(!operationQueue.isEmpty()) {
                IOperation* op = check_and_cast<IOperation*>(operationQueue.pop());
                propagate(op);
                if(!operationQueue.isEmpty()) {
                    simtime_t d = simTime() + SimTime(round(par("thinkTime").doubleValue()), SIMTIME_MS);
                    scheduleAt(d, thinkTimer);
                } else { controllerStatus = false; }
            } else { controllerStatus = false; }
        } else {
            throw cRuntimeError("Invalid timer msg: kind=%s", msg->str().c_str());
        }
    }
}


L3Address& HttpClientController::getNextIp() {
    const char* connectAddr = par("connectAddress");

    L3AddressResolver().tryResolve(connectAddr, ipAddress);
    if(ipAddress.isUnspecified()) {
        EV_ERROR << "Connecting to " << connectAddr << " : cannot resolve destination address\n";
    }
    return ipAddress;
}

void HttpClientController::next(Packet* packet) {
    this->controlFSM->next(packet);
}

void HttpClientController::propagate(IOperation* op) {
    emit(this->cmdPubSig, op);
}

void HttpClientController::evalRes(IResult* res) {

}

void HttpClientController::scheduleEvent(cMessage* event, simtime_t delay) {
    Enter_Method("Possible scheduling by Attack graph");
    take(event);
    cancelEvent(event);
    // Schedule event after delay from now
    simtime_t sTime = simTime() + delay;
    scheduleAt(sTime, event);
}

void HttpClientController::descheduleEvent(cMessage* event) {
    Enter_Method("Possible descheduling by Attack graph");
    cancelEvent(event);
}

void HttpClientController::enqueueNSchedule(IOperation* operation) {
    if(!controllerStatus) {
        operationQueue.insert(operation);
        simtime_t d = simTime() + SimTime(round(par("thinkTime").doubleValue()), SIMTIME_MS);
        scheduleAt(d, thinkTimer);
        controllerStatus = true;
    } else {
        operationQueue.insert(operation);
    }
}

HttpClientController::~HttpClientController() {
    cancelAndDelete(connectionTimer);
    cancelAndDelete(disconnectionTimer);
    cancelAndDelete(timeoutTimer);
    cancelAndDelete(sendRequestTimer);
    cancelAndDelete(startingTimer);
    cancelAndDelete(thinkTimer);
}
