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

#include "MmsClientResult.h"
#include "../../controller/client/MmsClientController.h"

using namespace inet;

MmsClientResult::MmsClientResult(int opId, ResultOutcome result) {
	this->opId = opId;
	this->result = result;
}

MmsClientResult::~MmsClientResult() {
	// TODO Auto-generated destructor stub
}

IController* MmsClientResult::getControllerOwner() {
	return this->controllerOwner;
}

void MmsClientResult::setControllerOwner(IController* controllerOwner) {
	MmsClientController* owner  = dynamic_cast<MmsClientController*>(controllerOwner);
	if(owner) this->controllerOwner = owner;
	else throw std::invalid_argument("controllewOwner must be of type MmsClientController");
}

