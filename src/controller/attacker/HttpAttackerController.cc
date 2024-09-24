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

#include "HttpAttackerController.h"

#include "../listener/MsgListener.h"
#include "../listener/ResListener.h"
#include "../fsm/factory/concrete/HttpAttackerFSMFactory.h"

using namespace inet;

#define MSGKIND_CONNECT    1
#define MSGKIND_SEND       2
#define MSGKIND_CLOSE      3

Define_Module(HttpAttackerController);

void HttpAttackerController::initialize() {
    ControllerBinder* binder = getBinder();
    binder->registerController(this);

    // Initialize operation, result and packet signals
    const char* strCmdPubSig = "httpAtkCmdSig";
    cmdPubSig = registerSignal(strCmdPubSig);

    const char* strResSubSig = "httpAtkResSig";
    const char* strMsgSubSig = "httpAtkMsgSig";
    resListener = new ResListener(this);
    msgListener = new MsgListener(this);
    // Subscribe listeners on the right module and signal
    getParentModule()->getParentModule()->subscribe(strResSubSig, resListener);
    getParentModule()->getParentModule()->subscribe(strMsgSubSig, msgListener);


    // Initialize network address space vector
    int maxNetSpace = par("maxNetSpace").intValue();
    std::string netIpPrefix = par("netIpPrefix").stdstringValue();

    for(int i = 0; i < maxNetSpace; i++) {
        std::string complAddr = netIpPrefix + std::string(".") + std::to_string(i);
        Ipv4Address ipv4Addr = Ipv4Address(complAddr.c_str());
        L3Address addr = L3Address(ipv4Addr);
        addrSpaceVector.push_back(addr);
    }
    nextAddrIdx = 0;

    controllerStatus = false;

    this->fsmFactory = new HttpAttackerFSMFactory(this);
    this->controlFSM = this->fsmFactory->build();
}

void HttpAttackerController::handleMessage(cMessage *msg) {
    if(msg == connectionTimer || msg == timeoutTimer) {
        this->controlFSM->next(msg);
    } else {
        throw cRuntimeError("Invalid timer msg: kind=%s", msg->str().c_str());
    }
}

void HttpAttackerController::scheduleNextTcpConnect() {

}

void HttpAttackerController::next(Packet* packet) {

}

void HttpAttackerController::propagate(IOperation* op) {
    emit(this->cmdPubSig, op);
}

void HttpAttackerController::evalRes(IResult* res) {

}

void HttpAttackerController::scheduleEvent(cMessage* event, simtime_t delay) {
    Enter_Method("Possible scheduling by Attack graph");
    take(event);
    cancelEvent(event);
    // Schedule event after delay from now
    simtime_t sTime = simTime() + delay;
    scheduleAt(sTime, event);
}

void HttpAttackerController::descheduleEvent(cMessage* event) {
    Enter_Method("Possible descheduling by Attack graph");
    cancelEvent(event);
}

void HttpAttackerController::enqueueNSchedule(IOperation* operation) {

}
