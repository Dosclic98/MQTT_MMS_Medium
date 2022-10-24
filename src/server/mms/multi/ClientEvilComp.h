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

	virtual ~ClientEvilComp();


protected:
	ChunkQueue queue;
	ChunkQueue tmpQueue;
	FromClientListener* serverCompListener;

	// Gestione segnali canali interni
	simsignal_t pcktFromServerSignal;

	//Gestione richiesta di misure e risposta casuale
	bool isListening;
	simsignal_t genericResponseSignal;

protected:
	virtual void initialize(int stage) override;
	virtual void sendRequest() override;
	virtual void handleTimer(cMessage* msg) override;
	virtual void socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) override;
	virtual void socketEstablished(TcpSocket *socket) override;

};

} // namespace inet

#endif
