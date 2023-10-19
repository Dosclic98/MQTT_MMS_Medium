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

#include "SendTcpConnectAtk.h"

using namespace inet;

void SendTcpConnectAtk::execute() {
	ClientEvilOperator* oper = check_and_cast<ClientEvilOperator*>(operatorOwner);
	oper->sendTcpConnect(this->id);
}

SendTcpConnectAtk::SendTcpConnectAtk() {
	this->id = ++SendTcpConnectAtk::idCounter;
}

SendTcpConnectAtk::~SendTcpConnectAtk() {
	// TODO Auto-generated destructor stub
}

