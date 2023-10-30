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
	AttackType attackType;
	bool state;
	const char* activationDelayExpr;
	std::vector<std::string> targetControllerList;
	std::vector<std::string> children;
};

class AttackGraph : public omnetpp::cModule, public IGraph {
  protected:
	std::map<std::string, AttackNode*> nodesMap;
	NodeContent nodes[8] = {
			{ "NetworkBegin", NodeType::BEGIN, AttackType::NOTSET, true, "uniform(2,4)", { }, { "Networkaccess" } },
			{ "Networkaccess", NodeType::STEP, AttackType::ACCESS, false, "uniform(10,20)", { "attacker.attackerController[0]", "attacker.attackerController[1]" }, { "ChanneladvInTheMid" } },
			{ "NetworktlsSet", NodeType::DEFENSE, AttackType::NOTSET, false, "uniform(4,6)", { }, { "ChanneladvInTheMid" } },
			{ "ChanneladvInTheMid", NodeType::STEP, AttackType::ADVINTHEMID, false, "uniform(0,1)", { "client.clientController[0]", "client.clientController[1]" }, { "DataFlowwrite" } },
			{ "DataFlowwrite", NodeType::STEP, AttackType::WRITEOP, false, "uniform(20,30)", { "attacker.attackerController[0]", "attacker.attackerController[1]" }, { "MMSServerspoRepMes" } },
			{ "MMSServerspoRepMes", NodeType::STEP, AttackType::SPOOFREPMES, false, "uniform(50,60)", { }, { "IEDpowSysacc" } },
			{ "IEDpowSysacc", NodeType::OR, AttackType::NOTSET, false, "uniform(4,6)", { }, { "PowerSystemEnd" } },
			{ "PowerSystemEnd", NodeType::END, AttackType::NOTSET, false, "uniform(4,6)", { }, { } }
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
