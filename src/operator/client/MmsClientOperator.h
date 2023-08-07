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

#ifndef __TX_MEDIUM_EXP_MMSCLIENTOPERATOR_H_
#define __TX_MEDIUM_EXP_MMSCLIENTOPERATOR_H_

#include <omnetpp.h>
#include "inet/applications/tcpapp/TcpBasicClientApp.h"
#include "../../utils/logger/mms/MmsPacketLogger.h"
#include "../IOperator.h"
#include "../listener/MmsOpListener.h"


namespace inet {


class MmsClientOperator : public TcpBasicClientApp, public IOperator
{
public:
	  virtual void sendMmsConnect(int opId);
	  virtual void sendMmsDisconnect(int opId);
	  virtual void sendMmsRequest(int opId, ReqResKind reqKind, int data);

private:
	  ChunkQueue queue;
	  int resTimeout;
	  MmsPacketLogger* logger = nullptr;

	  std::map<msgid_t, cMessage*> readResTimeoutMap;
	  std::map<msgid_t, cMessage*> commandResTimeoutMap;
	  std::map<msgid_t, simtime_t> genReqSentTimeMap;


	  //Gestione conteggio pacchetti nel range temporale
	  int measureCounter;
	  int measureAmountEventDelay = 4;
	  cMessage* measureAmountEvent;
	  simsignal_t measureReceivedCount;

	  //Gestione richiesta di misure e risposta casuale
	  bool isListening;
	  simsignal_t readSentSignal;
	  simsignal_t commandSentSignal;
	  simsignal_t readResponseSignal;
	  simsignal_t commandResponseSignal;
	  simsignal_t readResponseTimeoutSignal;
	  simsignal_t commandResponseTimeoutSignal;
	  simsignal_t readResponseReceivedTimeSignal;
	  simsignal_t commandResponseReceivedTimeSignal;


	  // Variable to know if the client is logging
	  bool isLogging;


	  virtual void initialize(int stage) override;
	  virtual void sendRequest(MMSKind kind = MMSKind::CONNECT, ReqResKind reqKind = ReqResKind::READ, int data = 0);
	  virtual void socketEstablished(TcpSocket *socket) override;
	  virtual void rescheduleOrDeleteTimer(simtime_t d, short int msgKind);
	  virtual void handleTimer(cMessage* msg) override;
	  virtual void socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) override;
	  virtual ~MmsClientOperator();

	  virtual void propagate(IResult* res) override;
	  virtual void propagate(MmsMessage* msg) override;
};

} // namespace inet

#endif
