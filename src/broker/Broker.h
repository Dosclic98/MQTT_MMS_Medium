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

#ifndef __MQTT_TCP_BROKER_H_
#define __MQTT_TCP_BROKER_H_


#include "inet/common/lifecycle/LifecycleUnsupported.h"
#include "inet/common/packet/ChunkQueue.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
//#include "inet/applications/common/SocketTag_m.h"
#include "inet/common/socket/SocketTag_m.h"
#include "../message/mqtt/MqttMessage_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/packet/Message.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/TimeTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/tcp/TcpCommand_m.h"

namespace inet {

class INET_API Broker : public cSimpleModule, public LifecycleUnsupported
{
  protected:
    TcpSocket socket;
    long msgsRcvd;
    long msgsSent;
    long bytesRcvd;
    long bytesSent;
    std::map<int, ChunkQueue> socketQueue;

    //Parametri Aggiuntivi
    bool brokerStatus;
    cQueue brokerQueue;
    std::map<int, std::list<int>> topicSocketIdMap;
    cMessage* brokerDepEvent;
    simsignal_t reportBlocked;
    simsignal_t commandBlocked;
    simsignal_t reportCompromised;
    simsignal_t commandCompromised;
    simsignal_t reportCountSignal;
    simsignal_t commandCountSignal;

  protected:
    virtual void sendBack(cMessage *msg);
    virtual void sendOrSchedule(cMessage *msg, simtime_t delay);
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;

    //Metodi Aggiuntivi
    virtual bool maliciousProgram(int topic);
    virtual void handleDeparture();
};

};

#endif
