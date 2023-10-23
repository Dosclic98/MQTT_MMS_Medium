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
	NodeType type;
	std::vector<std::string> children;
};

class AttackGraph : public omnetpp::cModule, public IGraph {
  protected:
	std::map<std::string, AttackNode*> nodesMap;
	NodeContent nodes[7] = {
			{ "NetworkBegin", NodeType::BEGIN, { "Networkaccess" } },
			{ "Networkaccess", NodeType::STEP, { "ChannelnetSni", "ChanneladvInTheMid" } },
			{ "NetworktlsSet", NodeType::DEFENSE, { "ChannelnetSni", "ChanneladvInTheMid" } },
			{ "ChannelnetSni", NodeType::STEP, { "IEDpowSysacc" } },
			{ "ChanneladvInTheMid", NodeType::STEP, { "IEDpowSysacc" } },
			{ "IEDpowSysacc", NodeType::OR, { "PowerSystemEnd" } },
			{ "PowerSystemEnd", NodeType::END, {  } }
	};

    virtual void initialize() override;

  private:
    virtual void connectNodes(AttackNode* startNode, AttackNode* endNode);

};

} // namespace inet

#endif
