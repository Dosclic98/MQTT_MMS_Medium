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

#include "HttpClientOperator.h"
#include "../../result/common/HttpClientResult.h"
#include "../listener/FromSerOpListener.h"
#include "../../message/http/HttpMessage_m.h"

using namespace inet;

#define MSGKIND_CONNECT    1
#define MSGKIND_SEND       2
#define MSGKIND_CLOSE      3

Define_Module(HttpClientOperator);

void HttpClientOperator::initialize(int stage) {
    TcpAppBase::initialize(stage);

    if(stage == INITSTAGE_LOCAL) {
        const char* strResPubSig = "httpResSig";
        const char* strMsgPubSig = "httpMsgSig";
        // Initializing inherited signals
        resPubSig = registerSignal(strResPubSig);
        msgPubSig = registerSignal(strMsgPubSig);
        cmdListener = new OpListener(this);
        const char* strCliCmdSig = "httpCmdSig";
        // Go up of two levels in the modules hierarchy (the first is the host module)
        getParentModule()->getParentModule()->subscribe(strCliCmdSig, cmdListener);
    }
}

void HttpClientOperator::socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) {
    EV_INFO << "Packet arrived!" << "\n";
    EV_INFO << msg->getKind() << "\n";
    if(msg->getKind() == TCP_I_DATA) {
        propagate(msg);
    }
}

void HttpClientOperator::handleTimer(cMessage *msg) {

}

void HttpClientOperator::socketEstablished(TcpSocket *socket) {
    EV_INFO << "Socket CONNECTED" << "\n";
    // The data of this message is irrelevant
    auto data = makeShared<ByteCountChunk>(B(1));
    Packet* connectedSocketMsg = new Packet("Socket connected", data);
    connectedSocketMsg->setKind(MSGKIND_CONNECT);
    propagate(connectedSocketMsg);
}

void HttpClientOperator::socketAvailable(TcpSocket* socket, TcpAvailableInfo *availableInfo) {
    EV_INFO << "Socket AVAILABLE" << "\n";
}

void HttpClientOperator::socketClosed(TcpSocket *socket) {
    EV_INFO << "Socket CLOSED" << "\n";
    // The data of this message is irrelevant
    auto data = makeShared<ByteCountChunk>(B(1));
    Packet* closedSocketMsg = new Packet("Socket closed", data);
    closedSocketMsg->setKind(MSGKIND_CLOSE);
    propagate(closedSocketMsg);
}

void HttpClientOperator::sendTcpConnect(int opId, L3Address& address) {
    Enter_Method("Initializing TCP connection");

    if(!address.isUnspecified()) {
        // we need a new connId if this is not the first connection
        socket.renewSocket();

        const char *localAddress = par("localAddress");
        int localPort = par("localPort");
        socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);

        int connectPort = par("connectPort");
        socket.connect(address, connectPort);

        // Propagate operation result
        propagate(new HttpClientResult(opId, ResultOutcome::SUCCESS));

    } else {
        propagate(new HttpClientResult(opId, ResultOutcome::FAIL));
    }
}

void HttpClientOperator::sendHttpRequest(int opId, Ptr<HttpRequestMessage> request) {
    Enter_Method("Sending HTTP request");
    Packet* packet = new Packet("HTTP Request");
    packet->insertAtBack(request);
    sendPacket(packet);

    propagate(new HttpClientResult(opId, ResultOutcome::SUCCESS));
}

void HttpClientOperator::handleTcpConnection(int opId) {
    // Nothing to be done other than propagating the operation result
    Enter_Method("Managing connected TCP Socket");
    propagate(new HttpClientResult(opId, ResultOutcome::SUCCESS));
}

void HttpClientOperator::handleTcpDisconnection(int opId) {
    // Nothing to be done other than propagating the operation result
    Enter_Method("Managing disconnected TCP Socket");
    propagate(new HttpClientResult(opId, ResultOutcome::SUCCESS));
}

void HttpClientOperator::handleTcpConnectTimeout(int opId) {
    Enter_Method("Closing dangling TCP Connection");
    close();
    propagate(new HttpClientResult(opId, ResultOutcome::SUCCESS));
}

void HttpClientOperator::sendTcpDisconnect(int opId) {
    Enter_Method("Closing TCP Connection");
    close();
    propagate(new HttpClientResult(opId, ResultOutcome::SUCCESS));
}

void HttpClientOperator::socketPeerClosed(TcpSocket *socket_) {
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

// TODO Understand better when this function is called
void HttpClientOperator::socketFailure(TcpSocket* socket, int code) {
    // subclasses may override this function, and add code try to reconnect after a delay.
    EV_WARN << "connection broken\n";

}

void HttpClientOperator::close() {
    EV_INFO << "Issuing CLOSE command\n";
    socket.close();
}

HttpClientOperator::~HttpClientOperator() {

}

void HttpClientOperator::propagate(IResult* res) {
    emit(this->resPubSig, res);
}

void HttpClientOperator::propagate(Packet* msg) {
    emit(this->msgPubSig, msg);
}
