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

#include "MmsClientOperation.h"

using namespace inet;

int IOperation::idCounter = 0;

IOperator* MmsClientOperation::getOperatorOwner() {
	if(auto* oper = dynamic_cast<MmsClientOperator*>(operatorOwner)) { return oper; }
	else throw std::invalid_argument("operatorOwner must be a subtype of MmsClientOperator");
}

void MmsClientOperation::setOperatorOwner(IOperator* operatorOwner) {
	MmsClientOperator* oper = dynamic_cast<MmsClientOperator*>(operatorOwner);
	if(oper) {this->operatorOwner = operatorOwner;}
	else { throw std::invalid_argument("operatorOwner must be a subtype of MmsClientOperator"); }
}
