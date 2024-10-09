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

#include "ManageHttpTcpSocketOpenClose.h"

using namespace inet;

void ManageHttpTcpSocketOpenClose::execute() {
    auto* operat = dynamic_cast<HttpClientOperator*>(this->operatorOwner);
    if(!operat) { operat = dynamic_cast<HttpClientEvilOperator*>(this->operatorOwner); }
    if(!operat) { throw std::invalid_argument("operatorOwner must be of type HttpClientOperator or HttpClientEvilOperator"); }

    if(this->open) {
        operat->handleTcpConnection(this->id);
    } else {
        operat->handleTcpDisconnection(this->id);
    }
}

ManageHttpTcpSocketOpenClose::ManageHttpTcpSocketOpenClose(bool open) {
    this->id = ++ManageHttpTcpSocketOpenClose::idCounter;
    this->open = open;
}

ManageHttpTcpSocketOpenClose::~ManageHttpTcpSocketOpenClose() {
    // TODO Auto-generated destructor stub
}

