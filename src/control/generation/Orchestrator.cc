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

#include "Orchestrator.h"

Define_Module(Orchestrator);

void Orchestrator::initialize() {
    // Create different INET modules and connections into the network where the module is contained
	cModule* cli = createMMSClient();
	cModule* ser = createMMSServer();
	cModule* router = createRouter();
	addConnections(cli, ser, router);
}

void Orchestrator::handleMessage(cMessage *msg) {
    // TODO - Generated method body
}

cModule* Orchestrator::createStandardHost(const char* hostName) {
	cModuleType* moduleType = cModuleType::get("inet.node.inet.StandardHost");
	cModule* cli = moduleType->create(hostName, getParentModule());
	cli->par("hasUdp") = false;
	return cli;
}

cModule* Orchestrator::createMMSClient() {
	cModule* cli = createStandardHost("MMSClient");
	cli->par("numApps") = 1;
	cli->buildInside();
	cli->setGateSize("ethg", 1);
	cModule* app = cli->getSubmodule("app", 0);
	return cli;
}


cModule* Orchestrator::createMMSServer() {
	cModule* cli = createStandardHost("MMSServer");
	cli->par("numApps") = 1;
	cli->finalizeParameters();
	cli->buildInside();
	cli->setGateSize("ethg", 1);
	cModule* firstApp = cli->getSubmodule("app", 0);
	// If the parameter is mutable is can be set in this way as an expression
	// cDynamicExpression* intExpr = new cDynamicExpression();
	// intExpr->parse("intuniform(47ms, 52ms)");
	// firstApp->par("thinkTime").setExpression(intExpr);

	return cli;
}

cModule* Orchestrator::createRouter() {
	cModuleType* routerType = cModuleType::get("inet.node.inet.Router");
	cModule* router = routerType->createScheduleInit("router", getParentModule());
	return router;
}

bool Orchestrator::addConnections(cModule* cli, cModule* ser, cModule* router) {
	cDatarateChannel *cliSer = cDatarateChannel::create("channel");
	cliSer->setDelay(0.001);
	cliSer->setDatarate(1e9);
	cDatarateChannel *serCli = cDatarateChannel::create("channel");
	serCli->setDelay(0.001);
	serCli->setDatarate(1e9);
	cli->gate("ethg$o", 0)->connectTo(ser->gate("ethg$i", 0), cliSer);
	ser->gate("ethg$o", 0)->connectTo(cli->gate("ethg$i", 0), serCli);

	return true;
}
