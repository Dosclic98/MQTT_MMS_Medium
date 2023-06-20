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

#ifndef __MQTT_TCP_A_H_
#define __MQTT_TCP_A_H_

#include <omnetpp.h>
#include "inet/applications/tcpapp/TcpBasicClientApp.h"
#include "inet/common/INETDefs.h"
#include "../../message/mms/MmsMessage_m.h"
#include "../../utils/logger/mms/MmsPacketLogger.h"

namespace inet {

class MmsClient: public TcpBasicClientApp {
  protected:
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
    //virtual void sendRequest() override;
    virtual void sendRequest(MMSKind kind = MMSKind::CONNECT, ReqResKind reqKind = ReqResKind::READ);
    virtual void rescheduleOrDeleteTimer(simtime_t d, short int msgKind);
    virtual void handleTimer(cMessage* msg) override;
    virtual void socketEstablished(TcpSocket* socket) override;
    virtual void socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) override;

  public:
    MmsClient() {}
    virtual ~MmsClient();
};

} // namespace inet

#endif
