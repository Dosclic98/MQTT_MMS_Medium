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

#include "HttpCommonOperation.h"

using namespace inet;

IOperator* HttpCommonOperation::getOperatorOwner() {
    if(auto* oper = dynamic_cast<HttpClientEvilOperator*>(operatorOwner)) { return oper; }
    else if(auto* oper = dynamic_cast<HttpServerOperator*>(operatorOwner)) { return oper; }
    // TODO Add here the HttpClientOperator cast when implemented
    else throw std::invalid_argument("operatorOwner must be of type HttpClientEvilOperator or HttpServerOperator");
}

void HttpCommonOperation::setOperatorOwner(IOperator* operatorOwner) {

    if(auto* oper = dynamic_cast<HttpClientEvilOperator*>(operatorOwner)) {this->operatorOwner = operatorOwner;}
    else if(auto* oper = dynamic_cast<HttpServerOperator*>(operatorOwner)) {this->operatorOwner = operatorOwner;}
    // TODO Add here the HttpClientOperator cast when implemented
    else { throw std::invalid_argument("operatorOwner must be of type HttpClientEvilOperator or HttpServerOperator"); }
}

