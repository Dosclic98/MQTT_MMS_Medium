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

#include "HttpClientResult.h"
#include "../../controller/attacker/HttpAttackerController.h"
#include "../../controller/client/HttpClientController.h"

using namespace inet;

HttpClientResult::HttpClientResult(int opId, ResultOutcome result) {
    this->opId = opId;
    this->result = result;
}

HttpClientResult::~HttpClientResult() {
    // TODO Auto-generated destructor stub
}

IController* HttpClientResult::getControllerOwner() {
    return this->controllerOwner;
}

void HttpClientResult::setControllerOwner(IController* controllerOwner) {
    if(auto* owner = dynamic_cast<HttpClientController*>(controllerOwner)) {this->controllerOwner = owner;}
    else if(auto* owner = dynamic_cast<HttpAttackerController*>(controllerOwner)) {this->controllerOwner = owner;}
    else throw std::invalid_argument("controllerOwner must be of type HttpClientController or HttpAttackerController");
}

