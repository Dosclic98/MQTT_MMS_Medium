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

#include "ForwardMmsMessageToServer.h"

using namespace inet;

ForwardMmsMessageToServer::ForwardMmsMessageToServer(Packet* msg) {
	this->id = ++ForwardMmsMessageToServer::idCounter;
	this->msg = msg;
}

ForwardMmsMessageToServer::~ForwardMmsMessageToServer() {
	// TODO Auto-generated destructor stub
}

void ForwardMmsMessageToServer::execute() {
	ClientEvilOperator* oper = check_and_cast<ClientEvilOperator*>(operatorOwner);
	oper->forwardToServer(id, msg);
}

ForwardMmsMessageToServer* ForwardMmsMessageToServer::build(Packet* packet) {

}
