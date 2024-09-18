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
#include "../listener/FromSerOpListener.h"

using namespace inet;

#define MSGKIND_CONNECT    1
#define MSGKIND_SEND       2
#define MSGKIND_CLOSE      3

Define_Module(HttpClientEvilOperator);

void HttpClientEvilOperator::initialize(int stage) {
    TcpAppBase::initialize(stage);

    if(stage == INITSTAGE_LOCAL) {
        const char* strResPubSig = "httpAtkResSig";
        const char* strMsgPubSig = "httpAtkMsgSig";
        // Initializing inherited signals
        resPubSig = registerSignal(strResPubSig);
        msgPubSig = registerSignal(strMsgPubSig);
        cmdListener = new OpListener(this);
        const char* strCliCmdSig = "httpAtkCmdSig";
        // Go up of two levels in the modules hierarchy (the first is the host module)
        getParentModule()->getParentModule()->subscribe(strCliCmdSig, cmdListener);
    } else if(stage == INITSTAGE_APPLICATION_LAYER) {
        int maxNetSpace = par("maxNetSpace").intValue();
        std::string netIpPrefix = par("netIpPrefix").stdstringValue();

        for(int i = 0; i < maxNetSpace; i++) {
            std::string complAddr = netIpPrefix + std::string(".") + std::to_string(i);
            Ipv4Address ipv4Addr = Ipv4Address(complAddr.c_str());
            L3Address addr = L3Address(ipv4Addr);
            addrSpaceVector.push_back(addr);
        }
        nextAddrIdx = 0;

    }
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
    closedSocketMsg->setKind(MSGKIND_CLOSE);
    propagate(closedSocketMsg);
}

void HttpClientEvilOperator::sendTcpConnect(int opId) {
    Enter_Method("Initializing TCP connection");
    if(nextAddrIdx < addrSpaceVector.size()) {
        L3Address addr = addrSpaceVector[nextAddrIdx];
        // we need a new connId if this is not the first connection
        socket.renewSocket();

        const char *localAddress = par("localAddress");
        int localPort = par("localPort");
        socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);

        int connectPort = par("connectPort");
        socket.connect(addr, connectPort);

        nextAddrIdx++;

        // Propagate operation result
        propagate(new HttpAttackerResult(opId, ResultOutcome::SUCCESS));
    } else {
        // TODO Probably just keep the indexes to which we could connect
        propagate(new HttpAttackerResult(opId, ResultOutcome::FAIL));
    }
}

void HttpClientEvilOperator::sendTcpDisconnect(int opId) {
    Enter_Method("Closing TCP Connection");
    close();
    propagate(new HttpAttackerResult(opId, ResultOutcome::SUCCESS));
}

void HttpClientEvilOperator::socketPeerClosed(TcpSocket *socket_) {
    ASSERT(socket_ == &socket);
    // close the connection (if not already closed)
    if (socket.getState() == TcpSocket::PEER_CLOSED) {
        EV_INFO << "Remote TCP closed, closing here as well\n";
        close();

        Packet* closedSocketMsg = new Packet("Socket closed");
        closedSocketMsg->setKind(MSGKIND_CLOSE);
        propagate(closedSocketMsg);
    }
}

// TODO Understand better when this functio is called
void HttpClientEvilOperator::socketFailure(TcpSocket* socket, int code) {
    // subclasses may override this function, and add code try to reconnect after a delay.
    EV_WARN << "connection broken\n";

}

void HttpClientEvilOperator::close() {
    EV_INFO << "Issuing CLOSE command\n";
    socket.close();
}

HttpClientEvilOperator::~HttpClientEvilOperator() {

}

void HttpClientEvilOperator::propagate(IResult* res) {
    emit(this->resPubSig, res);
}

void HttpClientEvilOperator::propagate(Packet* msg) {
    emit(this->msgPubSig, msg);
}
