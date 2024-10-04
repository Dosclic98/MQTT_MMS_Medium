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

#include "../../../attacker/concrete/ManageHttpTcpSocketOpenCloseAtk.h"

#define MSGKIND_CONNECT    1
#define MSGKIND_SEND       2
#define MSGKIND_CLOSE      3

using namespace inet;

void ManageHttpTcpSocketAtkFactory::build(Packet* packet) {
    HttpAttackerOperation* packetOp;
    if(packet->getKind() == MSGKIND_CONNECT) {
        // Create a ManageHttpTcpSocketOpenAtk operation
        packetOp = new ManageHttpTcpSocketOpenCloseAtk(true);
    } else if(packet->getKind() == MSGKIND_CLOSE) {
        // Create a ManageHttpTcpSocketCloseAtk operation
        packetOp = new ManageHttpTcpSocketOpenCloseAtk(false);
    } else {
        throw std::invalid_argument("Invalid packet as factory input");
    }
    delete packet;

    controller->enqueueNSchedule(packetOp);
}

ManageHttpTcpSocketAtkFactory::ManageHttpTcpSocketAtkFactory(HttpAttackerController* controller) {
    this->controller = controller;
}

ManageHttpTcpSocketAtkFactory::~ManageHttpTcpSocketAtkFactory() {
    // TODO Auto-generated destructor stub
}

