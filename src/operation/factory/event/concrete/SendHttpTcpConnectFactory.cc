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

#include "SendHttpTcpConnectFactory.h"

#include "../../../../controller/attacker/HttpAttackerController.h"
#include "../../../common/concrete/SendHttpTcpConnect.h"
#include "inet/networklayer/common/L3Address.h"

using namespace inet;

void SendHttpTcpConnectFactory::build(omnetpp::cEvent* event) {
    auto* controller = dynamic_cast<HttpClientController*>(this->controller);
    if(!controller && !(controller = dynamic_cast<HttpAttackerController*>(this->controller))) {
        throw std::invalid_argument("controller must be of type HttpAttackerController or HttpClientController");
    }

    L3Address& addr = controller->getNextIp();

    SendHttpTcpConnect* atkOp = new SendHttpTcpConnect(addr);
    controller->enqueueNSchedule(atkOp);
}

SendHttpTcpConnectFactory::SendHttpTcpConnectFactory(IController* controller) {
    if(!dynamic_cast<HttpAttackerController*>(controller) && !dynamic_cast<HttpClientController*>(controller)) {
        throw std::invalid_argument("controller must be of type HttpAttackerController or HttpClientController");
    }
    this->controller = controller;
}

SendHttpTcpConnectFactory::~SendHttpTcpConnectFactory() {
    // TODO Auto-generated destructor stub
}

