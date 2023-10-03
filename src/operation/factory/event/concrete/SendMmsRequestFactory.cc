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

#include "SendMmsRequestFactory.h"
#include "../../../../controller/client/MmsClientController.h"
#include "../../../client/concrete/SendMmsRequest.h"

using namespace inet;

void SendMmsRequestFactory::build(omnetpp::cEvent* event) {
	MmsClientController* controller = check_and_cast<MmsClientController*>(this->controller);
	cMessage* msg = check_and_cast<cMessage*>(event);
	switch(msg->getKind()) {
		case SEND_MMS_READ: {
			SendMmsRequest* cliOp = new SendMmsRequest(ReqResKind::READ, 0);
			controller->propagate(cliOp);

			// Schedule next Read send
		    controller->scheduleNextMmsRead();
		    break;
		}
		case SEND_MMS_COMMAND: {
			SendMmsRequest* cliOp = new SendMmsRequest(ReqResKind::COMMAND, 0);
			controller->propagate(cliOp);

		    // Schedule next Command send
			controller->scheduleNextMmsCommand();
		    break;
		}
	}
}

SendMmsRequestFactory::SendMmsRequestFactory(MmsClientController* controller) {
	this->controller = controller;
}

SendMmsRequestFactory::~SendMmsRequestFactory() { }

