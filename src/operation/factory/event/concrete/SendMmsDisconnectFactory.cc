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

#include "SendMmsDisconnectFactory.h"
#include "../../../../controller/client/MmsClientController.h"
#include "../../../client/concrete/SendMmsDisconnect.h"


using namespace inet;

void SendMmsDisconnectFactory::build(omnetpp::cEvent* event) {
	MmsClientController* controller = static_cast<MmsClientController*>(this->controller);
	SendMmsDisconnect* cliOp = new SendMmsDisconnect();
	controller->propagate(cliOp);
}

SendMmsDisconnectFactory::~SendMmsDisconnectFactory() { }

SendMmsDisconnectFactory::SendMmsDisconnectFactory(MmsClientController* controller) {
	this->controller = controller;
}

