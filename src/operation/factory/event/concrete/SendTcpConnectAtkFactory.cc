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

#include "SendTcpConnectAtkFactory.h"
#include "../../../attacker/concrete/SendTcpConnectAtk.h"

using namespace inet;

void SendTcpConnectAtkFactory::build(omnetpp::cEvent* event) {
	MmsAttackerController* controller = check_and_cast<MmsAttackerController*>(this->controller);
	SendTcpConnectAtk* atkOp = new SendTcpConnectAtk();
	controller->propagate(atkOp);
}

SendTcpConnectAtkFactory::SendTcpConnectAtkFactory(MmsAttackerController* controller) {
	this->controller = controller;
}

SendTcpConnectAtkFactory::~SendTcpConnectAtkFactory() {
	// TODO Auto-generated destructor stub
}

