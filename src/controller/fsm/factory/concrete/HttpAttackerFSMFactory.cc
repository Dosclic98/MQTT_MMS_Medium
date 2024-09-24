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

#include "HttpAttackerFSMFactory.h"
#include "../../../../controller/IController.h"
#include "../../../../controller/attacker/HttpAttackerController.h"
#include "../../state/concrete/OpState.h"
#include "../../operation/OpFSM.h"

using namespace inet;

HttpAttackerFSMFactory::HttpAttackerFSMFactory(HttpAttackerController* controller) {
    this->controller = controller;
}

IFSM* HttpAttackerFSMFactory::build() {
    // At the start the Attacker's CFSM has just a SCANNING state with no transitions
    HttpAttackerController* atkController = static_cast<HttpAttackerController*>(this->controller);
    OpState* scanningState = new OpState("SCANNING");

    OpFSM* fsm = new OpFSM(controller, scanningState);

    return fsm;
}

HttpAttackerFSMFactory::~HttpAttackerFSMFactory() {
    // TODO Auto-generated destructor stub
}

