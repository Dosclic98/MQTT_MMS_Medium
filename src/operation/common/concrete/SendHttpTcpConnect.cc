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

#include "SendHttpTcpConnect.h"

#include "inet/networklayer/common/L3Address.h"

using namespace inet;

void SendHttpTcpConnect::execute() {
    auto* operat = dynamic_cast<HttpClientOperator*>(this->operatorOwner);
    if(!operat) { operat = dynamic_cast<HttpClientEvilOperator*>(this->operatorOwner); }
    if(!operat) { throw std::invalid_argument("operatorOwner must be of type HttpClientOperator or HttpClientEvilOperator"); }


    if(!this->addr.isUnspecified()) {
        operat->sendTcpConnect(this->id, this->addr);
    } else {
        throw std::invalid_argument("Cannot execute SendHttpTcpConnectAtk operation on nullptr address");
    }

}

SendHttpTcpConnect::SendHttpTcpConnect(L3Address& addr) : addr(addr){
    this->id = ++SendHttpTcpConnect::idCounter;
}

SendHttpTcpConnect::~SendHttpTcpConnect() {
    // TODO Auto-generated destructor stub
}

