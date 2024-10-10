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

#include "HttpServerStationFSMFactory.h"
#include "../../state/concrete/OpState.h"
#include "../../transition/concrete/EventTransition.h"
#include "../../transition/concrete/PacketTransition.h"
#include "../../../server/HttpServerController.h"
#include "../../../../operation/factory/packet/concrete/SendHttpResponseFactory.h"
#include "../../concrete/OpFSM.h"

using namespace inet;

HttpServerStationFSMFactory::HttpServerStationFSMFactory(HttpServerController* controller) {
    this->controller = controller;
}

HttpServerStationFSMFactory::~HttpServerStationFSMFactory() {
    // TODO Auto-generated destructor stub
}

IFSM* HttpServerStationFSMFactory::build() {
    HttpServerController* controller = static_cast<HttpServerController*>(this->controller);
    OpState* operativeState = new OpState("OPERATIVE");

    std::vector<std::shared_ptr<ITransition>> operativeTransitions;
    std::shared_ptr<ITransition> operativeOk = std::make_shared<PacketTransition>(
            new SendHttpResponseFactory(controller, 200),
            operativeState,
            "content.method == 'POST' && content.targetUrl == '/api/login' && content.body == '{username=\"admin\",password=\"adminroot\"}'");
    std::shared_ptr<ITransition> operativeError = std::make_shared<PacketTransition>(
            new SendHttpResponseFactory(controller, 401),
            operativeState,
            "content.method != 'POST' || content.targetUrl != '/api/login' || content.body != '{username=\"admin\",password=\"adminroot\"}'");
    operativeTransitions.push_back(operativeOk);
    operativeTransitions.push_back(operativeError);
    operativeState->setTransitions(operativeTransitions);

    OpFSM* fsm = new OpFSM(controller, operativeState);

    return fsm;
}

