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

#include "SendTcpConnectFactory.h"
#include "../../../../controller/client/MmsClientController.h"
#include "../../../client/concrete/SendTcpConnect.h"

using namespace inet;

void SendTcpConnectFactory::build(omnetpp::cEvent* event) {
	MmsClientController* controller = static_cast<MmsClientController*>(this->controller);
	SendTcpConnect* cliOp = new SendTcpConnect(this->connectAddress);
	controller->propagate(cliOp);
	controller->scheduleNextMmsConnect();
}

SendTcpConnectFactory::SendTcpConnectFactory(MmsClientController* controller, std::string* connectAddress) {
	this->controller = controller;
	this->connectAddress = connectAddress;
}

SendTcpConnectFactory::~SendTcpConnectFactory() {

}

