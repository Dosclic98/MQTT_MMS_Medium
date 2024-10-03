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

#ifndef __MQTT_MMS_MEDIUM_HTTPATTACKERCONTROLLER_H_
#define __MQTT_MMS_MEDIUM_HTTPATTACKERCONTROLLER_H_

#include <omnetpp.h>
#include "../IController.h"
#include "inet/networklayer/common/L3Address.h"

namespace inet {
class HttpAttackerController : public cSimpleModule, public IController {
public:
    simtime_t connectTimeout;


    cMessage* connectionTimer = new cMessage("Connection Timer");
    cMessage* disconnectionTimer = new cMessage("Disconnection Timer");
    cMessage* timeoutTimer = new cMessage("Timeout Timer");
    cMessage* ipsFinishedTimer = new cMessage("IPs finished Timer");

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    virtual void next(Packet* msg = nullptr) override;
    virtual void propagate(IOperation* op) override;
    virtual void evalRes(IResult* res) override;
    virtual void scheduleEvent(cMessage* event, simtime_t delay) override;
    virtual void descheduleEvent(cMessage* event) override;
    virtual void enqueueNSchedule(IOperation* operation) override;
    virtual L3Address& getNextIp();
    virtual void saveCurrentIp();

protected:
    int maxNetSpace;
    std::string netIpPrefix;

    std::vector<L3Address> addrSpaceVector;
    int nextAddrIdx = 0;
    std::vector<L3Address> responsiveAddrVector;
};

} // namespace inet

#endif
