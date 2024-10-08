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

#include "SendHttpResponseFactory.h"
#include "../../../common/concrete/SendHttpResponse.h"
#include "inet/common/socket/SocketTag_m.h"
#include "inet/common/packet/Message.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "../../../../message/http/HttpMessage_m.h"
#include "../../../../utils/factories/http/HttpMessageFactory.h"

using namespace inet;

SendHttpResponseFactory::SendHttpResponseFactory(IController* controller, int result) {
    this->result = result;
    if(HttpServerController* cont = dynamic_cast<HttpServerController*>(controller)) {
        this->controller = cont;
    } else {
        throw std::invalid_argument("controller must be of type HttpServerController");
    }
}

SendHttpResponseFactory::~SendHttpResponseFactory() {
    // TODO Auto-generated destructor stub
}

void SendHttpResponseFactory::build(Packet* packet) {
    HttpServerController* controller = static_cast<HttpServerController*>(this->controller);
    ChunkQueue queue;
    HttpMessageFactory messageFactory;

    int connId = packet->getTag<SocketInd>()->getSocketId();
    auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
    queue.push(chunk);
    while (queue.has<HttpRequestMessage>(b(-1))) {
        const auto& reqPayload = queue.pop<HttpRequestMessage>(b(-1));

        // TODO Add some mechanism to not have to hardcode the message response generation
        const Ptr<HttpResponseMessage>& resPayload =
                messageFactory.buildResponse(this->result, reqPayload->getTargetUrl());

        SendHttpResponse* oper = new SendHttpResponse(connId, resPayload);
        controller->enqueueNSchedule(oper);
    }

}
