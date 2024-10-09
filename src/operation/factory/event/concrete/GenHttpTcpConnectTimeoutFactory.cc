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

#include "GenHttpTcpConnectTimeoutFactory.h"

#include "../../../../controller/attacker/HttpAttackerController.h"
#include "../../../common/concrete/GenHttpTcpConnectTimeout.h"

using namespace inet;

void GenHttpTcpConnectTimeoutFactory::build(omnetpp::cEvent* event) {
    GenHttpTcpConnectTimeout* op = new GenHttpTcpConnectTimeout();
    controller->enqueueNSchedule(op);
}

GenHttpTcpConnectTimeoutFactory::GenHttpTcpConnectTimeoutFactory(IController* controller) {
    if(!dynamic_cast<HttpAttackerController*>(controller) && !dynamic_cast<HttpClientController*>(controller)) {
        throw std::invalid_argument("controller must be of type HttpAttackerController or HttpClientController");
    }
    this->controller = controller;
}

GenHttpTcpConnectTimeoutFactory::~GenHttpTcpConnectTimeoutFactory() {
    // TODO Auto-generated destructor stub
}

