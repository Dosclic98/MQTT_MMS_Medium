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

#define MSGKIND_CONNECT    0
#define MSGKIND_DISCONNECT    2

Define_Module(HttpClientEvilOperator);

void HttpClientEvilOperator::initialize(int stage) {

}

void HttpClientEvilOperator::handleStartOperation(LifecycleOperation *operation) {

}

void HttpClientEvilOperator::handleTimer(cMessage* msg) {

}

void HttpClientEvilOperator::socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) {

}

void HttpClientEvilOperator::socketEstablished(TcpSocket *socket) {

}

void HttpClientEvilOperator::socketClosed(TcpSocket *socket) {

}

void HttpClientEvilOperator::sendTcpConnect(int opId) {
    Enter_Method("Initializing TCP connection");
    connect();
    propagate(new HttpAttackerResult(opId, ResultOutcome::SUCCESS));
}

void HttpClientEvilOperator::sendTcpDisconnect(int opId) {

}

HttpClientEvilOperator::~HttpClientEvilOperator() {

}

void HttpClientEvilOperator::propagate(IResult* res) {
    emit(this->resPubSig, res);
}

void HttpClientEvilOperator::propagate(Packet* msg) {
    emit(this->msgPubSig, msg);
}
