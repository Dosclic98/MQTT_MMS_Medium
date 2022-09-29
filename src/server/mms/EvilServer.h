
#ifndef __MQTT_TCP_EvilServer_H_
#define __MQTT_TCP_EvilServer_H_


#include "inet/common/lifecycle/LifecycleUnsupported.h"
#include "inet/common/packet/ChunkQueue.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "../../message/mms/MmsMessage_m.h"

namespace inet {

/**
 * Generic server application. It serves requests coming in GenericAppMsg
 * request messages. Clients are usually subclassed from TcpAppBase.
 *
 * @see GenericAppMsg, TcpAppBase
 */
class INET_API EvilServer : public cSimpleModule, public LifecycleUnsupported
{
  protected:
    TcpSocket socket;
    long msgsRcvd;
    long msgsSent;
    long bytesRcvd;
    long bytesSent;
    std::map<int, ChunkQueue> socketQueue;
    int serverConnId;

    //Gestione segnali
    simsignal_t measureBlockSignal;
    simsignal_t measureCompromisedSignal;
    simsignal_t genericRequestBlockSignal;
    simsignal_t genericRequestCompromisedSignal;
    simsignal_t genericResponseBlockSignal;
    simsignal_t genericResponseCompromisedSignal;

    //Gestione servitore e coda
    bool evilServerStatus;
    cQueue serverQueue;
    cMessage* departureEvent;

    //Gestione pacchetti da inviare al server
    std::list<MmsMessage> delayedPkts;

  protected:
    virtual void sendBack(cMessage *msg);
    virtual void sendOrSchedule(cMessage *msg, simtime_t delay);

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;
    //--
    virtual void handleDeparture();
    virtual void sendPacketDeparture(int connId, B requestedBytes, B replyLength, int messageKind, int clientConnId);
};

} // namespace inet


#endif

