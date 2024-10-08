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

#include "HttpMessageFactory.h"
#include "inet/common/TimeTag_m.h"

using namespace inet;

HttpMessageFactory::HttpMessageFactory() { }

HttpMessageFactory::~HttpMessageFactory() {
    // TODO Auto-generated destructor stub
}

const Ptr<HttpRequestMessage> HttpMessageFactory::buildRequest(const char* method, const char* targetUrl, bool keepAlive,
                                                      int protocol, const char* contentType,
                                                      int contentLength, const char* body) {
    const Ptr<HttpRequestMessage> reqMessage = makeShared<HttpRequestMessage>();
    reqMessage->setMethod(method);
    reqMessage->setTargetUrl(targetUrl);
    reqMessage->setKeepAlive(keepAlive);
    reqMessage->setProtocol(protocol);
    reqMessage->setContentType(contentType);
    reqMessage->setContentLength(contentLength);
    reqMessage->setBody(body);
    // Compute chunk length based on the sum of the lengths of each field
    reqMessage->setChunkLength(B(sizeof(method) + sizeof(targetUrl) + sizeof(keepAlive) +
                                sizeof(protocol) +  sizeof(contentType) +
                                sizeof(contentLength) + sizeof(body) + 16));
    reqMessage->addTag<CreationTimeTag>()->setCreationTime(simTime());
    return reqMessage;
}

const Ptr<HttpResponseMessage> HttpMessageFactory::buildResponse(int result, const char* originatorUrl,
                                                                    int protocol, const char* contentType,
                                                                    int contentLength, const char* body) {
    const Ptr<HttpResponseMessage> resMessage = makeShared<HttpResponseMessage>();
    resMessage->setResult(result);
    resMessage->setOriginatorUrl(originatorUrl);
    resMessage->setProtocol(protocol);
    resMessage->setContentType(contentType);
    resMessage->setContentLength(contentLength);
    resMessage->setBody(body);
    // Compute chunk length based on the sum of the lengths of each field
    resMessage->setChunkLength(B(sizeof(result) + sizeof(originatorUrl) +
                                sizeof(protocol) +  sizeof(contentType) +
                                sizeof(contentLength) + sizeof(body) + 16));
    resMessage->addTag<CreationTimeTag>()->setCreationTime(simTime());
    return resMessage;

}
