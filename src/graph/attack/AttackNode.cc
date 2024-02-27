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

#include "AttackNode.h"
#include "../../controller/client/MmsClientController.h"
#include "../../controller/attacker/MmsAttackerController.h"
#include "../../controller/fsm/operation/OpFSM.h"
#include "../../controller/fsm/state/concrete/OpState.h"
#include "../../controller/fsm/transition/concrete/EventTransition.h"
#include "../../controller/fsm/transition/concrete/PacketTransition.h"
#include "../../operation/factory/packet/concrete/ForwardMmsMessageToServerFactory.h"
#include "../../operation/factory/packet/concrete/ForwardMmsMessageToClientFactory.h"
#include "../../operation/factory/event/concrete/SendTcpConnectAtkFactory.h"
#include "../../operation/factory/event/concrete/SendTcpConnectFactory.h"
#include "../../operation/factory/event/concrete/SendMmsDisconnectFactory.h"
#include "../../operation/factory/event/concrete/SendMmsConnectFactory.h"
#include "../../operation/factory/event/concrete/SendMmsRequestFactory.h"

using namespace inet;

Define_Module(AttackNode);

void AttackNode::initialize() {
	// By default an AttackNode is not completed
	completedState = false;
    if(this->isActive()) {
    	if(this->getNodeType() == NodeType::BEGIN || this->getNodeType() == NodeType::DEFENSE) {
    		scheduleAt(omnetpp::simTime() + omnetpp::SimTime(par("activationDelay").doubleValue(), omnetpp::SIMTIME_S), new omnetpp::cMessage("Activate", KIND_ACTIVE));
    	} else {
    		throw std::invalid_argument("A node of type different from BEGIN or DEFENSE is active on initialization");
    	}
    }
}

// TODO Maybe implement node deactivation in the future
void AttackNode::handleMessage(omnetpp::cMessage *msg) {
    if(msg->isSelfMessage()) {
    	if(msg->getKind() == KIND_ACTIVE) {
    		delete msg;
        	// The current completion time has expired so the attack step is completed (notify all children)
    		this->completedState = true;
        	for(int i = 0; i < this->gateSize("out"); i++) {
        		this->send(new omnetpp::cMessage("Notify activation", KIND_NOTIFY_ACTIVE), "out", i);
        	}
    	}
    } else {
    	if(msg->getKind() == KIND_NOTIFY_ACTIVE) {
    		delete msg;
    		// A parent has been activated
    		if(!this->isActive()) {
    			updateActivation();
    			if(this->isActive()) {
    				this->executeStep();
    				scheduleAt(omnetpp::simTime() + omnetpp::SimTime(par("activationDelay").doubleValue(), omnetpp::SIMTIME_S), new omnetpp::cMessage("Activate", KIND_ACTIVE));
    			}
    		}
    	}
    }
}

void AttackNode::updateActivation() {
	std::vector<AttackNode*> parents = getParents();
	// Defense check
	for(AttackNode *parent : parents) {
		// If a parent defense is active the node cannot be activated
		if(parent->getNodeType() == NodeType::DEFENSE && parent->isActive() &&
				this->getNodeType() == NodeType::STEP) {
			this->state = false;
			return;
		}
	}

	// Other parents check
	bool toActivate = false;
	for(AttackNode *parent : parents) {
		toActivate = parent->isCompleted();
		if(this->getNodeType() == NodeType::AND) {
			if(!toActivate) break;
		} else if(this->getNodeType() == NodeType::OR || this->getNodeType() == NodeType::STEP ||
				this->getNodeType() == NodeType::END) {
			if(toActivate) break;
		}
	}

	this->state = toActivate;
}

void AttackNode::executeStep() {
	if(this->nodeType == NodeType::STEP) {
		for(IController* controller : this->targetControllers) {
			switch(this->attackType) {
				case AttackType::ACCESS: {
					MmsAttackerController* atkController = static_cast<MmsAttackerController*>(controller);
					OpState* unconnectedState = new OpState("UNCONNECTED");
					OpState* opState = new OpState("OPERATIVE");

					std::vector<std::shared_ptr<ITransition>> unconnectedTransitions;
					unconnectedTransitions.push_back(std::make_shared<EventTransition>(
						new SendTcpConnectAtkFactory(atkController),
						opState,
						new cMessage("TCPCONNECT", MSGKIND_CONNECT),
						EventMatchType::Kind,
						SimTime(1, SIMTIME_S)
					));
					unconnectedState->setTransitions(unconnectedTransitions);

					OpFSM* fsm = new OpFSM(controller, unconnectedState, false);
					atkController->getControlFSM()->merge(fsm);
					break;
				}
				case AttackType::ADVINTHEMID: {
					MmsClientController* cliController = static_cast<MmsClientController*>(controller);

					OpState* operatingState = new OpState("OPERATING");
					OpState* terminatedState = new OpState("TERMINATED");
					OpState* atkConnectedState = new OpState("ATKCONNECTED");
					OpState* atkOperatingState = new OpState("ATKOPERATING");

					// Create the operating transitions (add disconnect)
					std::vector<std::shared_ptr<ITransition>> operatingTransitions;
					operatingTransitions.push_back(std::make_shared<EventTransition>(
							new SendMmsDisconnectFactory(cliController),
							terminatedState,
							new cMessage("SENDDISCONNECT", SEND_MMS_DISCONNECT),
							EventMatchType::Kind,
							SimTime(1, SIMTIME_S)
					));
					operatingState->setTransitions(operatingTransitions);


					std::vector<std::shared_ptr<ITransition>> terminatedTransitions;
					std::string* strConnAddr = new std::string("IncrementalTest.attacker.attackerOperator");
					terminatedTransitions.push_back(std::make_shared<EventTransition>(
							new SendTcpConnectFactory(cliController, strConnAddr),
							atkConnectedState,
							new cMessage("TCPCONNECT", SEND_TCP_CONNECT),
							EventMatchType::Kind,
							SimTime(1, SIMTIME_S)
					));
					terminatedState->setTransitions(terminatedTransitions);

					// Create connected transitions
					std::vector<std::shared_ptr<ITransition>> atkConnectedTransitions;
					atkConnectedTransitions.push_back(std::make_shared<EventTransition>(
							new SendMmsConnectFactory(cliController),
							atkOperatingState,
							new cMessage("SENDMEAS", SEND_MMS_CONNECT),
							EventMatchType::Kind,
							SimTime(1, SIMTIME_S)
					));
					atkConnectedState->setTransitions(atkConnectedTransitions);

					// Create the operating transitions
					std::vector<std::shared_ptr<ITransition>> atkOperatingTransitions;
					atkOperatingTransitions.push_back(std::make_shared<EventTransition>(
							new SendMmsRequestFactory(cliController),
							atkOperatingState,
							new cMessage("SENDREAD", SEND_MMS_READ),
							EventMatchType::Kind,
							SimTime(cliController->par("sendReadInterval"), SIMTIME_S),
							cliController->par("sendReadInterval").getExpression()
					));
					atkOperatingTransitions.push_back(std::make_shared<EventTransition>(
							new SendMmsRequestFactory(cliController),
							atkOperatingState,
							new cMessage("SENDCOMMAND", SEND_MMS_COMMAND),
							EventMatchType::Kind,
							SimTime(cliController->par("sendCommandInterval"), SIMTIME_S),
							cliController->par("sendCommandInterval").getExpression()
					));
					atkOperatingState->setTransitions(atkOperatingTransitions);

					OpFSM* fsm = new OpFSM(controller, operatingState, false);

					cliController->getControlFSM()->merge(fsm);
					break;
				}
				case AttackType::WRITEOP: {
					MmsAttackerController* atkController = static_cast<MmsAttackerController*>(controller);
					OpState* opState = new OpState("OPERATIVE");

					std::vector<std::shared_ptr<ITransition>> operativeTransitions;
					operativeTransitions.push_back(std::make_shared<PacketTransition>(
						new ForwardMmsMessageToClientFactory(atkController),
						opState,
						"content.messageKind == 1 || content.messageKind == 3" // messageKind == MMSKind::MEASURE || messageKind == MMSKind::GENRESP
					));
					operativeTransitions.push_back(std::make_shared<PacketTransition>(
						new ForwardMmsMessageToServerFactory(atkController),
						opState,
						"content.messageKind == 0 || content.messageKind == 2" // messageKind == MMSKind::CONNECT || messageKind == MMSKind::GENREQ
					));

					opState->setTransitions(operativeTransitions);
					OpFSM* fsm = new OpFSM(controller, opState, false);
					atkController->getControlFSM()->merge(fsm);
					break;
				}
				default:
					EV << "No action defined for the activated attack step\n";
			}
		}
	}
	if(this->nodeType == NodeType::END) {
		// The attack has been completed
		this->endSimulation();
	}
}

std::vector<AttackNode*> AttackNode::getParents() {
	std::vector<AttackNode*> parentsVector(0);
	for(int i = 0; i < this->gateSize("in"); i++) {
		omnetpp::cGate* currGate = this->gate("in", i);
		omnetpp::cGate* prevGate = currGate->getPreviousGate();
		if(prevGate != nullptr) {
			AttackNode* parent = dynamic_cast<AttackNode*>(prevGate->getOwner());
			parentsVector.push_back(parent);
		}
	}

	return parentsVector;
}

void AttackNode::setNodeType(NodeType nodeType) {
	this->nodeType = nodeType;
}

NodeType AttackNode::getNodeType() {
	return this->nodeType;
}

bool AttackNode::isActive() {
	return this->state;
}

bool AttackNode::isCompleted() {
	return this->completedState;
}

void AttackNode::setState(bool state) {
	this->state = state;
}

void AttackNode::setAttackType(AttackType attackType) {
	this->attackType = attackType;
}

void AttackNode::setTargetControllers(std::vector<IController*> targetControllers) {
	this->targetControllers = targetControllers;
}

void AttackNode::refreshDisplay() const {
	cSimpleModule::refreshDisplay();

	omnetpp::cDisplayString& displayString = this->getDisplayString();
	if(state) {
		displayString.setTagArg("i", 1, "blue");
	} else {
		displayString.setTagArg("i", 1, "");
	}
}
