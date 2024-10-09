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
#include "../client/HttpClientController.h"

namespace inet {
class HttpAttackerController : public HttpClientController {
public:
    cMessage* ipsFinishedTimer = new cMessage("IPs finished Timer");
    cMessage* startingTimer = new cMessage("Starting connection");


    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    virtual L3Address& getNextIp() override;
    virtual void saveCurrentIp() override;
    virtual ~HttpAttackerController();

protected:
    int maxNetSpace;
    std::string netIpPrefix;

    std::vector<L3Address> addrSpaceVector;
    int nextAddrIdx = 0;
    std::vector<L3Address> responsiveAddrVector;
    int nextResAddrIdx = 0;
};

} // namespace inet

#endif
