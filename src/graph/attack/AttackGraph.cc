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

#include "AttackGraph.h"

using namespace inet;

Define_Module(AttackGraph);

void AttackGraph::initialize(int stage) {
	// On stage 0 we initialize the Attack nodes
	if(stage == 0) {
		const char* vectorName = "adjList";
		const char* activationDelayParName = "completionDelay";
		int vectorSize = 0;

		omnetpp::cModuleType* nodeType = omnetpp::cModuleType::get("tx_medium_exp.graph.attack.AttackNode");

		// Initialize nodes
		for(NodeContent nodeContent : nodes) {
			this->setSubmoduleVectorSize(vectorName, ++vectorSize);
			omnetpp::cModule* nodeModule = nodeType->create(vectorName, this, vectorSize - 1);
			AttackNode* nodeAttack = check_and_cast<AttackNode*>(nodeModule);
			// Set parameters
			nodeAttack->setNodeType(nodeContent.nodeType);
			nodeAttack->setState(nodeContent.state);
			nodeAttack->setAttackType(nodeContent.attackType);
			// If the activationDelayExpr is nullptr the default(0) is used
			if(nodeContent.completionDelayExpr != nullptr) {
				omnetpp::cDynamicExpression* activationDelayExpr = new omnetpp::cDynamicExpression();
				activationDelayExpr->parse(nodeContent.completionDelayExpr);
				nodeAttack->par(activationDelayParName).setExpression(activationDelayExpr);
			} else {
				if(nodeContent.nodeType == NodeType::BEGIN || nodeContent.nodeType == NodeType::DEFENSE) {
					throw std::invalid_argument("Incompatible completion delay expression for the node type");
				}
			}

			nodeAttack->finalizeParameters();
			nodeAttack->buildInside();
			// Set display name and display string
			nodeAttack->setDisplayName(nodeContent.displayName);
			omnetpp::cDisplayString& dispStr = nodeAttack->getDisplayString();
			dispStr.parse(AttackNode::displayStrings[nodeContent.nodeType].c_str());

			nodeAttack->scheduleStart(omnetpp::simTime());

			// Just to introduce the arcs more efficiently
			nodesMap.insert({nodeContent.displayName, nodeAttack});
		}

		// Initialize arcs
		for(NodeContent nodeContent : nodes) {
			AttackNode* startNodeAttack = nodesMap.at(nodeContent.displayName);
			for(std::string childName : nodeContent.children) {
				AttackNode* endNodeAttack = nodesMap.at(childName);
				// Connect the nodes if found
				this->connectNodes(startNodeAttack, endNodeAttack);
			}
		}
	}
	if(stage == 1) {
		// Initialize controller references
		ControllerBinder* controllerBinder = this->getBinder();
		for(NodeContent nodeContent : nodes) {
			AttackNode* attackNode = nodesMap.at(nodeContent.displayName);
			std::vector<IController*> targetControllers;
			for(std::string targetPathName : nodeContent.targetControllerList) {
				IController* targetController = controllerBinder->getRefByPathName(targetPathName.c_str());
				if(targetController == nullptr) throw std::invalid_argument("Invalid controller path name specified during initialization");
				targetControllers.push_back(targetController);
			}
			attackNode->setTargetControllers(targetControllers);
		}
	}
}

int AttackGraph::numInitStages() const {
	return 2;
}

void AttackGraph::connectNodes(AttackNode* startNode, AttackNode* endNode) {
	const char* inGate = "in";
	const char* outGate = "out";

	// Increase the gates sizes
	startNode->setGateSize(outGate, startNode->gateSize(outGate) + 1);
	endNode->setGateSize(inGate, endNode->gateSize(inGate) + 1);

	startNode->gate(outGate, startNode->gateSize(outGate) - 1)
			->connectTo(endNode->gate(inGate, endNode->gateSize(inGate) - 1));
}
