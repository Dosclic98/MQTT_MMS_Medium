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

#include "inet/common/INETDefs.h"

#include "inet/applications/tcpapp/TcpAppBase.h"
#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "../../message/mms/MmsMessage_m.h"
#include "../../utils/logger/mms/MmsPacketLogger.h"

namespace inet {

class INET_API MmsClient : public TcpAppBase
{
  protected:
    cMessage *timeoutMsg = nullptr;
    bool earlySend = false;    // if true, don't wait with sendRequest() until established()
    int numRequestsToSend = 0;    // requests to send in this session
    simtime_t startTime;
    simtime_t stopTime;
    ChunkQueue queue;
    int resTimeout;
    MmsPacketLogger* logger = nullptr;

    std::map<msgid_t, cMessage*> readResTimeoutMap;
    std::map<msgid_t, cMessage*> commandResTimeoutMap;

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

    // Variable to know if the client is logging
    bool isLogging;


    virtual void sendRequest(MMSKind kind = MMSKind::CONNECT, ReqResKind reqKind = ReqResKind::READ);
    virtual void rescheduleOrDeleteTimer(simtime_t d, short int msgKind);

    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleTimer(cMessage *msg) override;

    virtual void socketEstablished(TcpSocket *socket) override;
    virtual void socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) override;
    virtual void socketClosed(TcpSocket *socket) override;
    virtual void socketFailure(TcpSocket *socket, int code) override;

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    virtual void close() override;

  public:
    MmsClient() {}
    virtual ~MmsClient();
};

} // namespace inet

#endif
