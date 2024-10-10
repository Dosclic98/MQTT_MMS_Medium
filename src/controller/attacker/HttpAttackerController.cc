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
    getParentModule()->subscribe(strResSubSig, resListener);
    getParentModule()->subscribe(strMsgSubSig, msgListener);

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
    if(msg == ipsFinishedTimer) {
        this->controlFSM->next(msg);
    } else {
        HttpClientController::handleMessage(msg);
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

HttpAttackerController::~HttpAttackerController() {
    cancelAndDelete(ipsFinishedTimer);
}

