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

#include "PingEvilOperator.h"
#include "inet/networklayer/contract/ipv4/Ipv4Socket.h"

#define INIT_SOCKET 1

Define_Module(PingEvilOperator);

PingEvilOperator::~PingEvilOperator() {
    cancelAndDelete(timer);
    socketMap.deleteSockets();
}


void PingEvilOperator::initialize() {
    timer = new cMessage("Initialize socket", INIT_SOCKET);
    scheduleAt(simTime(), timer);
}

void PingEvilOperator::handleMessage(cMessage *msg) {
    // TODO This must be transfered within a new ping send
    if(msg->getKind() == INIT_SOCKET) {

    }
}

void PingEvilOperator::sendPing() {

}

void PingEvilOperator::bindNewSocket() {
    currentSocket = nullptr;
    currentSocket = new Ipv4Socket(gate("socketOut"));
    socketMap.addSocket(currentSocket);
    //currentSocket->bind(icmp, L3Address());
    currentSocket->setCallback(this);
}

void PingEvilOperator::handleMessageWhenUp(cMessage *msg) {

}

void PingEvilOperator::handleStartOperation(LifecycleOperation *operation) {

}

void PingEvilOperator::handleStopOperation(LifecycleOperation *operation) {

}

void PingEvilOperator::handleCrashOperation(LifecycleOperation *operation) {

}

void PingEvilOperator::socketDataArrived(INetworkSocket *socket, Packet *packet) {

}


void PingEvilOperator::socketClosed(INetworkSocket *socket) {
    if (socket == currentSocket)
        currentSocket = nullptr;
    delete socketMap.removeSocket(socket);
}

void PingEvilOperator::propagate(IResult* res) {
    emit(this->resPubSig, res);
}

void PingEvilOperator::propagate(Packet* msg) {
    emit(this->msgPubSig, msg);
}

