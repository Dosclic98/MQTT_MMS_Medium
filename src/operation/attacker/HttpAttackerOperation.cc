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

#include "HttpAttackerOperation.h"

using namespace inet;

IOperator* HttpAttackerOperation::getOperatorOwner() {
    if(auto* oper = dynamic_cast<HttpClientEvilOperator*>(operatorOwner)) { return oper; }
    else throw std::invalid_argument("operatorOwner must be a subtype of HttpClientEvilOperator");
}

void HttpAttackerOperation::setOperatorOwner(IOperator* operatorOwner) {
    HttpClientEvilOperator* oper = dynamic_cast<HttpClientEvilOperator*>(operatorOwner);
    if(oper) {this->operatorOwner = operatorOwner;}
    else { throw std::invalid_argument("operatorOwner must be a subtype of HttpClientEvilOperator"); }
}

