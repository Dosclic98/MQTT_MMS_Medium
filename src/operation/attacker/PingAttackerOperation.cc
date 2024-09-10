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

#include "PingAttackerOperation.h"

using namespace inet;

IOperator* PingAttackerOperation::getOperatorOwner() {
    if(auto* operatorOwner = dynamic_cast<PingEvilOperator*>(this->operatorOwner)) {return operatorOwner;}
    else {throw std::invalid_argument("operatorOwner must be a subtype of PingEvilOperator");}
}

void PingAttackerOperation::setOperatorOwner(IOperator* operatorOwner) {
    PingEvilOperator* tmpOwner = dynamic_cast<PingEvilOperator*>(operatorOwner);
    if(tmpOwner) {this->operatorOwner = tmpOwner;}
    else {throw std::invalid_argument("operatorOwner must be a subtype of PingEvilOperator");}
}

