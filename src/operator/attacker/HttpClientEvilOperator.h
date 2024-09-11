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

#ifndef __MQTT_MMS_MEDIUM_HTTPCLIENTEVILOPERATOR_H_
#define __MQTT_MMS_MEDIUM_HTTPCLIENTEVILOPERATOR_H_

#include <omnetpp.h>
#include "inet/applications/tcpapp/TcpBasicClientApp.h"
#include "../IOperator.h"

namespace inet {

class HttpClientEvilOperator : public TcpBasicClientApp, public IOperator {
public:
    // The IOperator methods
    virtual void propagate(IResult* res) override;
    virtual void propagate(Packet* msg = nullptr) override;

    virtual void sendTcpConnect(int opId);
    virtual void sendTcpDisconnect(int opId);


    virtual ~HttpClientEvilOperator();

protected:
    virtual void initialize(int stage) override;
    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleTimer(cMessage* msg) override;
    virtual void socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) override;
    virtual void socketEstablished(TcpSocket *socket) override;
    virtual void socketClosed(TcpSocket *socket) override;
    // TODO Implement a method that calculates all addresses within a specific network space (look inside the PingApp)
};

}; // namespace inet

#endif
