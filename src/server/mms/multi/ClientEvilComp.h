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

#include "inet/applications/tcpapp/TcpBasicClientApp.h"

namespace inet {

/**
 * TODO - Generated class
 */
class ClientEvilComp : public TcpBasicClientApp
{
	protected:
		//Gestione conteggio pacchetti nel range temporale
		int counter;
		cMessage* topicAmountEvent;
		simsignal_t topicAmount;
		ChunkQueue queue;

		//Gestione richiesta di misure e risposta casuale
		bool isListening;
		bool previousResponseSent;
		simsignal_t genericResponseSignal;

	protected:
		virtual void initialize(int stage) override;
		virtual void sendRequest() override;
		virtual void socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) override;
		virtual void socketEstablished(TcpSocket *socket) override;

};

} // namespace inet

#endif
