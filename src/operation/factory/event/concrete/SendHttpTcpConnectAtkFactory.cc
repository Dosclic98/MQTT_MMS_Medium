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

#include "SendHttpTcpConnectAtkFactory.h"
#include "../../../../controller/attacker/HttpAttackerController.h"
#include "../../../attacker/concrete/SendHttpTcpConnectAtk.h"
#include "inet/networklayer/common/L3Address.h"

using namespace inet;

void SendHttpTcpConnectAtkFactory::build(omnetpp::cEvent* event) {
    HttpAttackerController* controller = check_and_cast<HttpAttackerController*>(this->controller);

    // TODO Get next adddress to initialize the operation
    if(nextAddrIdx >= addrSpaceVector.size()) {
        throw std::invalid_argument("Trying to access an inexistent IP");
    }
    L3Address addr = addrSpaceVector[nextAddrIdx];
    nextAddrIdx++;
    if(nextAddrIdx >= addrSpaceVector.size()) {
        // If there is no more next IP update dormancies
        controller->getControlFSM()->updateDormancy(controller->ipsFinishedTimer);
    }

    SendHttpTcpConnectAtk* atkOp = new SendHttpTcpConnectAtk(addr);
    controller->propagate(atkOp);
}

SendHttpTcpConnectAtkFactory::SendHttpTcpConnectAtkFactory(HttpAttackerController* controller) {
    this->controller = controller;

    // Initialize network address space vector
    int maxNetSpace = controller->maxNetSpace;
    std::string netIpPrefix = controller->netIpPrefix;

    for(int i = 0; i < maxNetSpace; i++) {
        std::string complAddr = netIpPrefix + std::string(".") + std::to_string(i);
        Ipv4Address ipv4Addr = Ipv4Address(complAddr.c_str());
        L3Address addr = L3Address(ipv4Addr);
        addrSpaceVector.push_back(addr);
    }
    nextAddrIdx = 0;
}

SendHttpTcpConnectAtkFactory::~SendHttpTcpConnectAtkFactory() {
    // TODO Auto-generated destructor stub
}

