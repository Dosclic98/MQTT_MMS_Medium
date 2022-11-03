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

#ifndef __MQTT_TCP_MMSSERVER_H_
#define __MQTT_TCP_MMSSERVER_H_


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
class INET_API MmsServer : public cSimpleModule, public LifecycleUnsupported
{
  protected:
    TcpSocket socket;
    long msgsRcvd;
    long msgsSent;
    long bytesRcvd;
    long bytesSent;
    std::map<int, ChunkQueue> socketQueue;
    std::list< std::pair<int,int> > clientConnIdList;

    //Gestione servitore e coda
    bool serverStatus;
    cQueue serverQueue;
    cMessage* departureEvent;

    //Gestione invio misure
    cMessage* sendDataEvent;

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
    virtual void sendPacketDeparture(int connId, int evilConnId, B requestedBytes, B replyLength, MMSKind messageKind);

  public:
    virtual ~MmsServer();
};

} // namespace inet


#endif

