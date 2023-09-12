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

#include "OpState.h"
#include "../../IFSM.h"
#include "../../../operation/IOperation.h"
#include "../../../IController.h"

using namespace inet;

OpState::~OpState() {
	// TODO Auto-generated destructor stub
}

OpState::OpState(std::vector<IState*> transitions, IOperation* operation, IController* targetController) {
	this->transitions = transitions;
	this->operation = operation;
	this->targetController = targetController;
}

IState* OpState::next(IFSM* machine, Packet* msg) {

}

IState* OpState::next(IFSM* machine, IResult* msg) {

}

