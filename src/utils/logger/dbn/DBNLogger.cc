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

#include "DBNLogger.h"
#include <experimental/filesystem>
#include "../../../graph/attack/AttackGraph.h"

namespace fs = std::experimental::filesystem;

using namespace inet;

Define_Module(DBNLogger);

void DBNLogger::initialize(int stage) {
    if(stage == INITSTAGE_LAST - 1) {
        cEnvir* ev = getSimulation()->getActiveEnvir();
        int runNumber = ev->getConfigEx()->getActiveRunNumber();

        // Initialize logger after the AG has been intialized
        std::string fileName = par("fileNamePrefix").stdstringValue() + std::string("-#") + std::to_string(runNumber) + std::string(".csv");

        ag = static_cast<AttackGraph*>(this->getParentModule());
        deltaT = par("deltaT").doubleValue();

        // Append the specific folder based on the run number at the path.
        std::string path = par("pathPrefix").stdstringValue() + std::string("/run") + std::to_string(runNumber) + std::string("/");

        createFoldersInPath(path);
        // Initialize the header
        logFile.open(path + fileName);
        const std::map<std::string, AttackNode*>& nodesMap = ag->getNodesMap();
        simtime_t simTimeLimit = SimTime::parse(ev->getConfig()->getConfigValue("sim-time-limit"));
        int numSteps = simTimeLimit / deltaT;
        for(std::pair<std::string, AttackNode*> elem : nodesMap) {
            for(int i = 0; i < numSteps; i++) {
                if(i == 0) {
                    logFile << elem.first << ",";
                } else if(i < numSteps-1) {
                    logFile << elem.first << "_" << i << ",";
                } else {
                    logFile << elem.first << "_" << i;
                }
            }
        }
        logFile << "\n";
    }
}

void DBNLogger::createFoldersInPath(std::string& path) {
    fs::create_directories(path); // create folders in path
}

void DBNLogger::notifyActivation(AttackNode* node) {
    nodeActivationMap[node] = simTime();
}

int DBNLogger::numInitStages() const {
    return 23;
}

void DBNLogger::finish() {
    cEnvir* ev = getSimulation()->getActiveEnvir();
    const std::map<std::string, AttackNode*>& nodesMap = ag->getNodesMap();
    simtime_t simTimeLimit = SimTime::parse(ev->getConfig()->getConfigValue("sim-time-limit"));

    for(std::pair<std::string, AttackNode*> elem : nodesMap) {
        int numSteps = simTimeLimit / deltaT;
        simtime_t nodeActivationTime = simTimeLimit;
        if(nodeActivationMap.find(elem.second) != nodeActivationMap.end()) {
            nodeActivationTime = nodeActivationMap[elem.second];
        }
        for(int i = 0; i < numSteps; i++) {
            simtime_t timeSliceTime = deltaT * (i+1);
            // Write N for "not completed" and C for "completed"
            char currNodeValue = timeSliceTime < nodeActivationTime ? 'N' : 'C';
            if(i < numSteps-1) {
                logFile << currNodeValue << ",";
            } else {
                logFile << currNodeValue;
            }
        }
    }
    logFile << "\n";
    logFile.close();
}
