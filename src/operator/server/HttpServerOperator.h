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

#ifndef __MQTT_MMS_MEDIUM_HTTPSERVEROPERATOR_H_
#define __MQTT_MMS_MEDIUM_HTTPSERVEROPERATOR_H_

#include <omnetpp.h>
#include "inet/applications/tcpapp/TcpGenericServerApp.h"
#include "../IOperator.h"
#include "../listener/OpListener.h"
#include "inet/common/packet/ChunkQueue.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"

namespace inet {

class HttpResponseMessage;

class HttpServerOperator : public TcpGenericServerApp, public IOperator {

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void sendBack(cMessage *msg) override;

public:
    virtual void sendHttpResponse(int opId, Packet* packet);

    virtual void propagate(IResult* res) override;
    virtual void propagate(Packet* msg) override;
};

}; // namespace inet

#endif
