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

#include "SendHttpRequestFactory.h"
#include "../../../../controller/attacker/HttpAttackerController.h"
#include "../../../common/concrete/SendHttpRequest.h"
#include "../../../../message/http/HttpMessage_m.h"

using namespace inet;

SendHttpRequestFactory::SendHttpRequestFactory(IController* controller, const Ptr<HttpRequestMessage> message) : message(message) {
    auto* castedController = dynamic_cast<HttpAttackerController*>(controller);
    // TODO Add cast check to HttpClientController
    if(castedController) {this->controller = castedController;}
    else {
        throw std::invalid_argument("controller must be of type HttpAttackerControllers"); ;
    }
}

SendHttpRequestFactory::~SendHttpRequestFactory() {
    // TODO Auto-generated destructor stub
}

void SendHttpRequestFactory::build(omnetpp::cEvent* event) {
    SendHttpRequest* requestOp = new SendHttpRequest(message);

    controller->enqueueNSchedule(requestOp);
}
