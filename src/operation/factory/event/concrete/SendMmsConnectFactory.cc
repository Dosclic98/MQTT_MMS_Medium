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

#include "SendMmsConnectFactory.h"
#include "../../../../controller/client/MmsClientController.h"
#include "../../../client/concrete/SendMmsConnect.h"

using namespace inet;

void SendMmsConnectFactory::build(omnetpp::cEvent* event) {
	MmsClientController* controller = static_cast<MmsClientController*>(this->controller);
	SendMmsConnect* cliOp = new SendMmsConnect();
	controller->propagate(cliOp);
	controller->scheduleNextMmsDisconnect();
}

SendMmsConnectFactory::SendMmsConnectFactory(MmsClientController* controller) {
	this->controller = controller;
}

SendMmsConnectFactory::~SendMmsConnectFactory() { }

