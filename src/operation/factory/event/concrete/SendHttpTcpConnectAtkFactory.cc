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

    L3Address& addr = controller->getNextIp();

    SendHttpTcpConnectAtk* atkOp = new SendHttpTcpConnectAtk(addr);
    controller->propagate(atkOp);
}

SendHttpTcpConnectAtkFactory::SendHttpTcpConnectAtkFactory(HttpAttackerController* controller) {
    this->controller = controller;
}

SendHttpTcpConnectAtkFactory::~SendHttpTcpConnectAtkFactory() {
    // TODO Auto-generated destructor stub
}

