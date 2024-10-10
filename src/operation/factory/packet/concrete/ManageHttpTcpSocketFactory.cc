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

#include "ManageHttpTcpSocketAtkFactory.h"
#include "../../../common/concrete/ManageHttpTcpSocketOpenClose.h"

#define MSGKIND_CONNECT    1
#define MSGKIND_SEND       2
#define MSGKIND_CLOSE      3

using namespace inet;

void ManageHttpTcpSocketAtkFactory::build(Packet* packet) {
    HttpCommonOperation* packetOp;

    if(packet->getKind() == MSGKIND_CONNECT) {
        // Create a ManageHttpTcpSocketOpenAtk operation
        packetOp = new ManageHttpTcpSocketOpenClose(true);
    } else if(packet->getKind() == MSGKIND_CLOSE) {
        // Create a ManageHttpTcpSocketCloseAtk operation
        packetOp = new ManageHttpTcpSocketOpenClose(false);
    } else {
        throw std::invalid_argument("Invalid packet as factory input");
    }
    delete packet;

    controller->enqueueNSchedule(packetOp);
}

ManageHttpTcpSocketAtkFactory::ManageHttpTcpSocketAtkFactory(IController* controller) {
    if(!dynamic_cast<HttpAttackerController*>(controller) && !dynamic_cast<HttpClientController*>(controller)) {
        throw std::invalid_argument("controller must be of type HttpAttackerController or HttpClientController");
    }
    this->controller = controller;
}

ManageHttpTcpSocketAtkFactory::~ManageHttpTcpSocketAtkFactory() {
    // TODO Auto-generated destructor stub
}

