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


#include "ClientEvilOperator.h"
#include "../listener/FromSerOpListener.h"
#include "../../message/mms/MmsMessage_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/socket/SocketTag_m.h"
#include <algorithm>

#include "../../result/attacker/MmsAttackerResult.h"

namespace inet {

#define MSGKIND_CONNECT    0
#define MSGKIND_SEND       1

Define_Module(ClientEvilOperator);

ClientEvilOperator::~ClientEvilOperator() {

}

void ClientEvilOperator::initialize(int stage)
{
    TcpAppBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        numRequestsToSend = 1000;
        WATCH(earlySend);
        WATCH(numRequestsToSend);
        startTime = par("startTime");
        stopTime = par("stopTime");
        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            throw cRuntimeError("Invalid startTime/stopTime parameters");
        timeoutMsg = new cMessage("timer");

        pcktFromServerSignal = registerSignal("pcktFromServerSignal");

        char strResPubSig[30];
        char strMsgPubSig[30];
        sprintf(strResPubSig, "atkResSig-%d", this->getIndex());
        sprintf(strMsgPubSig, "atkMsgSig-%d", this->getIndex());
        // Initializing inherited signals
        resPubSig = registerSignal(strResPubSig);
        msgPubSig = registerSignal(strMsgPubSig);
        cmdListener = new MmsOpListener(this);
        char strCliCmdSig[30];
        sprintf(strCliCmdSig, "atkCmdSig-%d", this->getIndex());
        // Go up of two levels in the modules hierarchy (the first is the host module)
        getParentModule()->getParentModule()->subscribe(strCliCmdSig, cmdListener);


        // Initialize listener and subscribe to the serverComp forwarding signal
        serverCompListener = new FromSerOpListener(this);
        char strSig[30];
        sprintf(strSig, "pcktFromClientSignal-%d", this->getIndex());
        getContainingNode(this)->subscribe(strSig, serverCompListener);
    }
}


void ClientEvilOperator::handleTimer(cMessage *msg) {
	switch (msg->getKind()) {
		case MSGKIND_CONNECT:
			connect();
			break;

		default:
			throw cRuntimeError("Invalid timer msg: kind=%d", msg->getKind());
	}
}

void ClientEvilOperator::socketEstablished(TcpSocket *socket) {
    TcpAppBase::socketEstablished(socket);

    // Signal to the controller the successfull socket connection
    propagate();
}

int ClientEvilOperator::getConnectionState() {
	return socket.getState();
}

void ClientEvilOperator::socketDataArrived(TcpSocket *socket, Packet *pckt, bool urgent) {
    if (socket->getState() == TcpSocket::LOCALLY_CLOSED) {
        EV_INFO << "reply to last request arrived, closing session\n";
        close();
        return;
    }
    propagate(pckt);
}

void ClientEvilOperator::forwardToClient(int opId, Packet* pckt) {
	emit(pcktFromServerSignal, pckt);
	propagate(new MmsAttackerResult(opId, ResultOutcome::SUCCESS));
}

void ClientEvilOperator::forwardToServer(int opId, Packet* pckt) {
	sendPacket(pckt);
	propagate(new MmsAttackerResult(opId, ResultOutcome::SUCCESS));
}

void ClientEvilOperator::propagate(IResult* res) {
	emit(this->resPubSig, res);
}

void ClientEvilOperator::propagate(Packet* msg) {
	emit(msgPubSig, msg);
}


}
