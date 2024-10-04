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
    maxNetSpace = par("maxNetSpace").intValue();
    netIpPrefix = par("netIpPrefix").stdstringValue();
    connectTimeout = par("connectTimeout");

    // Initialize network address space vector
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
    if(msg == connectionTimer || msg == timeoutTimer ||
            msg == disconnectionTimer || msg == ipsFinishedTimer ||
            msg == startingTimer) {
        this->controlFSM->next(msg);
    } else {
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


L3Address& HttpAttackerController::getNextIp() {
    // Get next adddress to initialize the send TCP connect operation
    if(nextAddrIdx < addrSpaceVector.size()) {
        L3Address& addr = addrSpaceVector[nextAddrIdx];
        nextAddrIdx++;
        if(nextAddrIdx >= addrSpaceVector.size()) {
            // If there is no more next IP update dormancies
            getControlFSM()->updateDormancy(ipsFinishedTimer);
        }
        return addr;
    } else {
        if(nextResAddrIdx < responsiveAddrVector.size()) {
            L3Address& addr = responsiveAddrVector[nextResAddrIdx];
            nextResAddrIdx++;
            return addr;
        }
        throw std::invalid_argument("Trying to access an inexistent IP");
    }
}

void HttpAttackerController::saveCurrentIp() {
    L3Address& currAddr = addrSpaceVector[nextAddrIdx-1];
    responsiveAddrVector.push_back(currAddr);
}

void HttpAttackerController::next(Packet* packet) {
    this->controlFSM->next(packet);
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
    if(!controllerStatus) {
        operationQueue.insert(operation);
        simtime_t d = simTime() + SimTime(round(par("thinkTime").doubleValue()), SIMTIME_MS);
        scheduleAt(d, thinkTimer);
        controllerStatus = true;
    } else {
        operationQueue.insert(operation);
    }
}

HttpAttackerController::~HttpAttackerController() {
    cancelAndDelete(connectionTimer);
    cancelAndDelete(disconnectionTimer);
    cancelAndDelete(timeoutTimer);
    cancelAndDelete(ipsFinishedTimer);
    cancelAndDelete(startingTimer);
    cancelAndDelete(thinkTimer);
}

