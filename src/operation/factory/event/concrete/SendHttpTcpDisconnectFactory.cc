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

#include "SendHttpTcpDisconnectFactory.h"

#include "../../../../controller/attacker/HttpAttackerController.h"
#include "../../../common/concrete/SendHttpTcpDisconnect.h"

using namespace inet;

void SendHttpTcpDisconnectFactory::build(omnetpp::cEvent* event) {
    auto* controller = dynamic_cast<HttpClientController*>(this->controller);
    if(!controller && !(controller = dynamic_cast<HttpAttackerController*>(this->controller))) {
        throw std::invalid_argument("controller must be of type HttpAttackerController or HttpClientController");
    }
    // Store the current IP address to which the attacker was connected
    if(this->store) {
        controller->saveCurrentIp();
    }
    SendHttpTcpDisconnect* atkOp = new SendHttpTcpDisconnect();
    controller->enqueueNSchedule(atkOp);
}


SendHttpTcpDisconnectFactory::SendHttpTcpDisconnectFactory(HttpAttackerController* controller, bool store) {
    if(!dynamic_cast<HttpAttackerController*>(controller) && !dynamic_cast<HttpClientController*>(controller)) {
        throw std::invalid_argument("controller must be of type HttpAttackerController or HttpClientController");
    }
    this->controller = controller;
    this->store = store;
}

SendHttpTcpDisconnectFactory::~SendHttpTcpDisconnectFactory() {
    // TODO Auto-generated destructor stub
}

