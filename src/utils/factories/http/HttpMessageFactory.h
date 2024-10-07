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

#ifndef UTILS_FACTORIES_HTTP_HTTPMESSAGEFACTORY_H_
#define UTILS_FACTORIES_HTTP_HTTPMESSAGEFACTORY_H_

#include <omnetpp.h>
#include "../../../message/http/HttpMessage_m.h"

namespace inet {

class HttpMessageFactory {
public:
    HttpMessageFactory();
    virtual ~HttpMessageFactory();

    virtual const Ptr<HttpRequestMessage> buildRequest(const char* method, const char* targetUrl, bool keepAlive = true,
                                                        int protocol = 11, const char* contentType="text/text",
                                                        int contentLength = 0, const char* body="");
};

}; // namespace inet

#endif /* UTILS_FACTORIES_HTTP_HTTPMESSAGEFACTORY_H_ */
