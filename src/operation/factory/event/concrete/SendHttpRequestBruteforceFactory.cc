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

#include "SendHttpRequestBruteforceFactory.h"
#include "../../../../controller/attacker/HttpAttackerController.h"
#include "../../../common/concrete/SendHttpRequest.h"
#include "../../../../message/http/HttpMessage_m.h"

using namespace inet;

SendHttpRequestBruteforceFactory::SendHttpRequestBruteforceFactory(IController* controller) {
    if(!dynamic_cast<HttpAttackerController*>(controller) && !dynamic_cast<HttpClientController*>(controller)) {
        throw std::invalid_argument("controller must be of type HttpAttackerController or HttpClientController");
    }
    this->controller = controller;
    this->passwordIndex = 0;
    shufflePasswords(this->passwordVector);
}

SendHttpRequestBruteforceFactory::~SendHttpRequestBruteforceFactory() {
    // TODO Auto-generated destructor stub
}

void SendHttpRequestBruteforceFactory::build(omnetpp::cEvent* event) {
    if(passwordIndex >= passwordVector.size()) {
        throw std::out_of_range("PasswordIndex out of range: The correct password was not in the dictionary");
    }
    std::string bodyStr =  std::string("{username:\"") + std::string(username) + std::string("\",password:\"") +
            std::string(passwordVector[passwordIndex]) + std::string("\"}");
    const char* body = strdup(bodyStr.c_str());
    Ptr<HttpRequestMessage> httpRequest = messageFactory.buildRequest("POST", "/api/login",
                                            true, 11, "text/json", sizeof(body), body);
    SendHttpRequest* oper = new SendHttpRequest(httpRequest);
    controller->enqueueNSchedule(oper);
    passwordIndex++;
}

void SendHttpRequestBruteforceFactory::shufflePasswords(std::vector<std::string>& passwords) {
    auto* controller = dynamic_cast<HttpClientController*>(this->controller);
    if(!controller && !(controller = dynamic_cast<HttpAttackerController*>(this->controller))) {
        throw std::invalid_argument("controller must be of type HttpAttackerController or HttpClientController");
    }
    // For N times swap two random elements in the vector
    for(int i = 0; i < passwords.size(); i++) {
        int a  = controller->intuniform(0, passwords.size()-1);
        int b = controller->intuniform(0, passwords.size()-1);
        std::string tmp = passwords[a];
        passwords[a] = passwords[b];
        passwords[b] = tmp;
    }
}
