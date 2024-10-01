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

#include "ManageHttpTcpSocketOpenCloseAtk.h"

using namespace inet;

void ManageHttpTcpSocketOpenCloseAtk::execute() {
    HttpClientEvilOperator* operat = check_and_cast<HttpClientEvilOperator*>(this->operatorOwner);

    if(this->open) {
        operat->handleTcpConnection(this->id);
    } else {
        operat->handleTcpDisconnection(this->id);
    }
}

ManageHttpTcpSocketOpenCloseAtk::ManageHttpTcpSocketOpenCloseAtk(bool open) {
    this->id = ++ManageHttpTcpSocketOpenCloseAtk::idCounter;
    this->open = open;
}

ManageHttpTcpSocketOpenCloseAtk::~ManageHttpTcpSocketOpenCloseAtk() {
    // TODO Auto-generated destructor stub
}

