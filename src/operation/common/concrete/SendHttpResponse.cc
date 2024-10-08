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

#include "SendHttpResponse.h"
#include "../../../message/http/HttpMessage_m.h"
#include "inet/transportlayer/contract/tcp/TcpCommand_m.h"
#include "inet/common/socket/SocketTag_m.h"

using namespace inet;

SendHttpResponse::SendHttpResponse(int conId, const Ptr<HttpResponseMessage> resMessage) :
        resMessage(resMessage) {
    this->id = ++SendHttpResponse::idCounter;
    this->connId = conId;
}

SendHttpResponse::~SendHttpResponse() {
    // TODO Auto-generated destructor stub
}

void SendHttpResponse::execute() {
    auto *oper = dynamic_cast<HttpServerOperator*>(operatorOwner);
    assert(oper != nullptr);

    Packet* resPacket = new Packet("Http response", TCP_C_SEND);
    resPacket->addTag<SocketReq>()->setSocketId(this->connId);
    resPacket->insertAtBack(resMessage);

    oper->sendHttpResponse(id, resPacket);
}

