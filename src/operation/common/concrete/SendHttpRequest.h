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

#ifndef OPERATION_COMMON_CONCRETE_SENDHTTPREQUEST_H_
#define OPERATION_COMMON_CONCRETE_SENDHTTPREQUEST_H_

#include "../HttpCommonOperation.h"

namespace inet {

class HttpRequestMessage;

class SendHttpRequest : public HttpCommonOperation {
public:
    SendHttpRequest(const Ptr<HttpRequestMessage> reqMessage);
    virtual ~SendHttpRequest();

    virtual void execute() override;

protected:
    const Ptr<HttpRequestMessage> reqMessage;
};

}; // namespace inet

#endif /* OPERATION_COMMON_CONCRETE_SENDHTTPREQUEST_H_ */
