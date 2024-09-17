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

#include "HttpClientEvilOperator.h"
#include "../../result/attacker/HttpAttackerResult.h"

using namespace inet;

#define MSGKIND_CONNECT    1
#define MSGKIND_SEND       2
#define MSGKIND_CLOSE      3

Define_Module(HttpClientEvilOperator);

void HttpClientEvilOperator::initialize(int stage) {
    TcpAppBase::initialize(stage);

    int maxNetSpace = par("maxNetSpace").intValue();
    std::string netIpPrefix = par("netIpPrefix").stdstringValue();
    int connectPort = par("connectPort");

    for(int i = 0; i < maxNetSpace; i++) {
        std::string complAddr = netIpPrefix + std::to_string(i);
        L3Address addr = L3Address(complAddr.c_str());
        addrSpaceVector.push_back(addr);
        EV << addr.toIpv4() << "\n";
    }
    nextAddr = 0;
}

void HttpClientEvilOperator::socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) {

}

void HttpClientEvilOperator::handleMessageWhenUp(cMessage *msg) {

}

void HttpClientEvilOperator::handleTimer(cMessage *msg) {

}

void HttpClientEvilOperator::socketEstablished(TcpSocket *socket) {
    Packet* connectedSocketMsg = new Packet("Socket connected");
    connectedSocketMsg->setKind(MSGKIND_CONNECT);
    propagate(connectedSocketMsg);
}

void HttpClientEvilOperator::socketClosed(TcpSocket *socket) {
    Packet* closedSocketMsg = new Packet("Socket closed");
    closedSocketMsg->setKind(MSGKIND_CONNECT);
    propagate(closedSocketMsg);
}

void HttpClientEvilOperator::sendTcpConnect(int opId) {
    Enter_Method("Initializing TCP connection");



    propagate(new HttpAttackerResult(opId, ResultOutcome::SUCCESS));
}

void HttpClientEvilOperator::sendTcpDisconnect(int opId) {
    Enter_Method("Closing TCP Connection");
    close();
    propagate(new HttpAttackerResult(opId, ResultOutcome::SUCCESS));
}

HttpClientEvilOperator::~HttpClientEvilOperator() {

}

void HttpClientEvilOperator::propagate(IResult* res) {
    emit(this->resPubSig, res);
}

void HttpClientEvilOperator::propagate(Packet* msg) {
    emit(this->msgPubSig, msg);
}
