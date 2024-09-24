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

#include "SendHttpTcpConnectAtk.h"
#include "inet/networklayer/common/L3Address.h"

using namespace inet;

void SendHttpTcpConnectAtk::execute() {
    HttpClientEvilOperator* oper = check_and_cast<HttpClientEvilOperator*>(operatorOwner);

    if(this->addr != nullptr) {
        oper->sendTcpConnect(this->id, this->addr);
    } else {
        throw std::invalid_argument("Cannot execute SendHttpTcpConnectAtk operation on nullptr address");
    }

}

SendHttpTcpConnectAtk::SendHttpTcpConnectAtk() {
    this->id = ++SendHttpTcpConnectAtk::idCounter;
}

SendHttpTcpConnectAtk::SendHttpTcpConnectAtk(L3Address* addr) : SendHttpTcpConnectAtk() {
    this->addr = addr;
}

SendHttpTcpConnectAtk::~SendHttpTcpConnectAtk() {
    // TODO Auto-generated destructor stub
}

