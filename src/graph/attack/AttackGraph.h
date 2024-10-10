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
	const char* completionDelayExpr;
	std::vector<std::string> targetControllerList;
	std::vector<std::string> children;
};

class AttackGraph : public omnetpp::cModule, public IGraph {
  protected:
	std::map<std::string, AttackNode*> nodesMap;
	NodeContent nodes[21] = {
	        { "workStation_compromise", NodeType::BEGIN, AttackType::NOTSET, true, "uniform(2,4)", { }, { "DMZ_scanIP", "historianServer_remoteShellAND" } },
	        { "DMZ_scanIP", NodeType::STEP, AttackType::SCANIP, false, nullptr, { "workstation.httpAttackerController" }, { "historian_scanVuln" } },
	        { "historian_scanVuln", NodeType::STEP, AttackType::SCANVULN, false, nullptr, { "workstation.httpAttackerController" }, { "historian_remoteSrvc" } },
	        { "historian_remoteSrvc", NodeType::STEP, AttackType::REMOTESRVC, false, "truncnormal(10, 2)", { }, { "historianServer_shell" } },
	        { "historianServer_shell", NodeType::STEP, AttackType::SHELL, false, "truncnormal(8, 1)", { }, { "historianServer_NodeOR", "historianServer_addSSHKey" } },
	        { "historianServer_addSSHKey", NodeType::STEP, AttackType::ADDSSHKEY, false, "truncnormal(9, 3)", { }, { "historianServer_remoteShellAND", "historianServer_addSSHKey" } },
	        { "historianServer_remoteShellAND", NodeType::AND, AttackType::NOTSET, false, nullptr, { }, { "historianServer_remoteShell" } },
	        { "historianServer_remoteShell", NodeType::STEP, AttackType::REMOTESHELL, false, nullptr, { }, { "historianServer_NodeOR"} },
	        { "historianServer_NodeOR", NodeType::OR, AttackType::NOTSET, false, nullptr, { }, { "tomcatWebServer_bruteForce" } },
	        { "tomcatWebServer_bruteForce", NodeType::STEP, AttackType::BRUTEFORCE, false, nullptr, { "historianServer.httpClientController" }, { "virtualEng_escapeHost" } },
	        { "virtualEng_escapeHost", NodeType::STEP, AttackType::ESCAPEHOST, false, nullptr, { }, { "stationComputer_shell" } },
	        { "stationComputer_shell", NodeType::STEP, AttackType::SHELL, false, nullptr, { }, { "MMSclient_unsecCred", "MMSclient_modAuthProc" } },
	        { "MMSclient_unsecCred", NodeType::STEP, AttackType::UNSECCRED, false, nullptr, { }, { "MMSclient_AND" } },
	        { "MMSclient_modAuthProc", NodeType::STEP, AttackType::MODAUTHPROC, false, nullptr, { }, { "MMSclient_AND" } },
	        { "MMSclient_AND", NodeType::AND, AttackType::NOTSET, false, nullptr, { }, { "MMSclient_credAcc" } },
	        { "MMSclient_credAcc", NodeType::STEP, AttackType::CREDACC, false, nullptr, { }, { "MMSTLS_AITM" } },
	        { "MMSTLS_AITM", NodeType::STEP, AttackType::AITM, false, nullptr, { }, { "MMS_unauthCmdMsg", "MMSclient_spoofRepMsg" } },
	        { "MMS_unauthCmdMsg", NodeType::STEP, AttackType::UNAUTHCMDMSG, false, nullptr, { }, { "IED_DERfailureOR" } },
	        { "MMSclient_spoofRepMsg", NodeType::STEP, AttackType::SPOOFREPMSG, false, nullptr, { }, { "IED_DERfailureOR" } },
	        { "IED_DERfailureOR", NodeType::OR, AttackType::NOTSET, false, nullptr, { }, { "IED_DERfailure" } },
	        { "IED_DERfailure", NodeType::END, AttackType::NOTSET, false, nullptr, { }, { } },

			// { "NetworkBegin", NodeType::BEGIN, AttackType::NOTSET, true, "uniform(2,4)", { }, { "Networkaccess" } },
			// { "Networkaccess", NodeType::STEP, AttackType::ACCESS, false, nullptr, { "attacker.attackerController[0]", "attacker.attackerController[1]" }, { "ChanneladvInTheMid" } },
			// { "NetworktlsSet", NodeType::DEFENSE, AttackType::NOTSET, false, "uniform(4,6)", { }, { "ChanneladvInTheMid" } },
			// { "ChanneladvInTheMid", NodeType::STEP, AttackType::ADVINTHEMID, false, nullptr, { "stationComputer.mmsClientController[0]", "stationComputer.mmsClientController[1]" }, { "DataFlowwrite" } },
			// { "DataFlowwrite", NodeType::STEP, AttackType::WRITEOP, false, nullptr, { "attacker.attackerController[0]", "attacker.attackerController[1]" }, { "IEDpowSysacc" } },
			// { "MMSServerspoRepMes", NodeType::STEP, AttackType::SPOOFREPMES, false, nullptr, { }, { "IEDpowSysacc" } },
			// { "IEDpowSysacc", NodeType::OR, AttackType::NOTSET, false, nullptr, { }, { "PowerSystemEnd" } },
			// { "PowerSystemEnd", NodeType::END, AttackType::NOTSET, false, nullptr, { }, { } }
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
