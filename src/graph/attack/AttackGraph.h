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

#ifndef __TX_MEDIUM_EXP_ATTACKGRAPH_H_
#define __TX_MEDIUM_EXP_ATTACKGRAPH_H_

#include <omnetpp.h>
#include "../IGraph.h"
#include "AttackNode.h"



namespace inet {

struct NodeContent {
	const char* displayName;
	NodeType nodeType;
	bool state;
	const char* activationDelayExpr;
	std::vector<std::string> targetControllerList;
	std::vector<std::string> children;
};

class AttackGraph : public omnetpp::cModule, public IGraph {
  protected:
	std::map<std::string, AttackNode*> nodesMap;
	NodeContent nodes[8] = {
			{ "NetworkBegin", NodeType::BEGIN, true, "uniform(4,6)", { }, { "Networkaccess" } },
			{ "Networkaccess", NodeType::STEP, false, "uniform(4,6)", { "attacker.attackerController[0]", "attacker.attackerController[1]" }, { "ChanneladvInTheMid" } },
			{ "NetworktlsSet", NodeType::DEFENSE, false, "uniform(4,6)", { }, { "ChanneladvInTheMid" } },
			{ "ChanneladvInTheMid", NodeType::STEP, false, "uniform(4,6)", { "client.clientController[0]", "client.clientController[1]" }, { "DataFlowwrite" } },
			{ "DataFlowwrite", NodeType::STEP, false, "uniform(4,6)", { }, { "MMSServerspoRepMes" } },
			{ "MMSServerspoRepMes", NodeType::STEP, false, "uniform(20,30)", { }, { "IEDpowSysacc" } },
			{ "IEDpowSysacc", NodeType::OR, false, "uniform(4,6)", { }, { "PowerSystemEnd" } },
			{ "PowerSystemEnd", NodeType::END, false, "uniform(4,6)", { }, { } }
	};

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override;
    virtual ControllerBinder* getBinder() {
        return check_and_cast<ControllerBinder*>(getSimulation()->getModuleByPath("controllerBinder"));
    }

  private:
    virtual void connectNodes(AttackNode* startNode, AttackNode* endNode);

};

} // namespace inet

#endif
