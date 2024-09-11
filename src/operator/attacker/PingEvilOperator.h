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

#ifndef __MQTT_MMS_MEDIUM_PINGEVILOPERATOR_H_
#define __MQTT_MMS_MEDIUM_PINGEVILOPERATOR_H_

#include <omnetpp.h>
#include "../IOperator.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/common/socket/SocketMap.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/common/Protocol.h"
#include "inet/networklayer/contract/INetworkSocket.h"

using namespace inet;

class PingEvilOperator : public ApplicationBase, public INetworkSocket::ICallback, public IOperator {

public:
    // The IOperator methods
    virtual void propagate(IResult* res) override;
    virtual void propagate(Packet* msg = nullptr) override;
    virtual void sendPing();
    virtual void bindNewSocket();

    virtual ~PingEvilOperator();

    std::vector<std::string> modulesToPing = {"router1", "pkiServer", "historianServer"};

protected:
    INetworkSocket* currentSocket = nullptr;
    SocketMap socketMap;
    cMessage *timer = nullptr; // to schedule socket initialization

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;
    virtual void socketDataArrived(INetworkSocket *socket, Packet *packet) override;
    virtual void socketClosed(INetworkSocket *socket) override;
};

#endif
