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

#ifndef __TX_MEDIUM_EXP_CLIENTEVILCOMP_H_
#define __TX_MEDIUM_EXP_CLIENTEVILCOMP_H_

#include <omnetpp.h>
#include <queue>
#include "inet/applications/tcpapp/TcpBasicClientApp.h"

namespace inet {

class FromClientListener;
/**
 * TODO - Generated class
 */
class ClientEvilComp : public TcpBasicClientApp {
public:
	cQueue msgQueue;
	bool previousResponseSent;
	cMessage* sendMsgEvent;
	virtual void rescheduleAfterOrDeleteTimer(simtime_t d, short int msgKind) override;
	virtual int getConnectionState();

	// Gestione segnali
	simsignal_t genericFakeReqResSignal;
	simsignal_t measureBlockSignal;
	simsignal_t measureCompromisedSignal;
	simsignal_t readResponseBlockSignal;
	simsignal_t readResponseCompromisedSignal;
	simsignal_t commandResponseBlockSignal;
	simsignal_t commandResponseCompromisedSignal;
	simsignal_t readRequestBlockSignal;
	simsignal_t readRequestCompromisedSignal;
	simsignal_t commandRequestBlockSignal;
	simsignal_t commandRequestCompromisedSignal;

	double readResponseBlockProb;
	double readResponseCompromisedProb;
	double commandResponseBlockProb;
	double commandResponseCompromisedProb;

	double readRequestBlockProb;
	double readRequestCompromisedProb;
	double commandRequestBlockProb;
	double commandRequestCompromisedProb;

	double measureBlockProb;
	double measureCompromisedProb;

	virtual ~ClientEvilComp();


protected:
	ChunkQueue queue;
	FromClientListener* serverCompListener;

	// Gestione segnali canali interni
	simsignal_t pcktFromServerSignal;

protected:
	virtual void initialize(int stage) override;
	virtual void sendRequest() override;
	virtual void handleTimer(cMessage* msg) override;
	virtual void socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) override;
	virtual void socketEstablished(TcpSocket *socket) override;

};

} // namespace inet

#endif
