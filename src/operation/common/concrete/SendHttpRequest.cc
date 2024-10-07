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

#include "SendHttpRequest.h"
#include "../../../message/http/HttpMessage_m.h"

using namespace inet;

SendHttpRequest::SendHttpRequest(const Ptr<HttpRequestMessage> reqMessage) : reqMessage(reqMessage) {
    this->id = ++SendHttpRequest::idCounter;
}

SendHttpRequest::~SendHttpRequest() {
    // TODO Auto-generated destructor stub
}

void SendHttpRequest::execute() {
    auto* oper = dynamic_cast<HttpClientEvilOperator*>(operatorOwner);
    if(oper == nullptr) {
        // TODO Add cast to HttpClientOperator
    }
    assert(oper != nullptr);

    oper->sendHttpRequest(id, reqMessage);
}

