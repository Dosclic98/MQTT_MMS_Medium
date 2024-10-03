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

#include "HttpServerResult.h"
#include "../../controller/server/HttpServerController.h"

using namespace inet;

HttpServerResult::HttpServerResult(int opId, ResultOutcome result) {
    this->opId = opId;
    this->result = result;
}

HttpServerResult::~HttpServerResult() {
    // TODO Auto-generated destructor stub
}

IController* HttpServerResult::getControllerOwner() {
    return this->controllerOwner;
}

void HttpServerResult::setControllerOwner(IController* controllerOwner) {
    HttpServerController* owner  = dynamic_cast<HttpServerController*>(controllerOwner);
    if(owner) this->controllerOwner = owner;
    else throw std::invalid_argument("controllewOwner must be of type HttpServerController");
}

