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

#ifndef __MQTT_MMS_MEDIUM_DBNLOGGER_H_
#define __MQTT_MMS_MEDIUM_DBNLOGGER_H_

#include <omnetpp.h>
#include <iostream>
#include <fstream>

using namespace omnetpp;

namespace inet {

class AttackGraph;
class AttackNode;

class DBNLogger : public cSimpleModule {
  public:
    virtual void notifyActivation(AttackNode* node);
  protected:
    std::string path = "logs/";
    std::ofstream logFile;
    AttackGraph* ag;
    simtime_t deltaT;

    std::map<AttackNode*, simtime_t> nodeActivationMap;

    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override {}
    virtual void finish() override;
    virtual int numInitStages() const override;
};

}; // namespace inet

#endif
