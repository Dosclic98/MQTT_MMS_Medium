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
	complTimeSignal = registerSignal("complTimeSignal");
	startTimeSignal = registerSignal("startTimeSignal");
	endTimeSignal = registerSignal("endTimeSignal");
    if(this->isActive()) {
    	if(this->getNodeType() == NodeType::BEGIN || this->getNodeType() == NodeType::DEFENSE) {
    	    stepStart = simTime();
    		scheduleCompletionDelay();
    	} else {
    		throw std::invalid_argument("A node of type different from BEGIN or DEFENSE is active on initialization");
    	}
    }
    // If a defense is not active we do not emit anything as completion time signal
}

// TODO Maybe implement node deactivation in the future
void AttackNode::handleMessage(omnetpp::cMessage *msg) {
    if(msg->isSelfMessage()) {
    	if(msg->getKind() == KIND_COMPLETED) {
    		delete msg;
        	// The current attack step has been completed completion delay has expired so the attack node is completed (notify all children)
    		this->completedState = true;
        	notifyCompletion();
        	// Emit completion time statistic
        	emit(complTimeSignal, simTime() - this->stepStart);
        	emit(endTimeSignal, simTime());
    	}
    } else {
    	if(msg->getKind() == KIND_NOTIFY_COMPLETED) {
    		delete msg;
    		// A parent has been activated
    		if(!this->isActive()) {
    			updateActivation();
    			if(this->isActive()) {
    			    stepStart = simTime();
    				this->executeStep();
    				// In the following case the canary is empty so just schedule a completion message
    				if(this->getNodeType() == NodeType::AND || this->getNodeType() == NodeType::OR) {
    					scheduleCompletionDelay();
    				}
    			}
    		}
    	}
    }
}

void AttackNode::scheduleCompletionDelay() {
	scheduleAt(omnetpp::simTime() + omnetpp::SimTime(par("completionDelay").doubleValue(), omnetpp::SIMTIME_S), new omnetpp::cMessage("Completed", KIND_COMPLETED));
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

void AttackNode::updateCanary(ITransition* trans) {
	Enter_Method("Updating canary status from controller context");
	auto extrTransition = this->completionCanary.find(trans);
	if(extrTransition != this->completionCanary.end()) {
		this->completionCanary[trans] = true;
		// Now check for attack step completion
		checkForCompletion();
	}
}

void AttackNode::checkForCompletion() {
	if(!completedState) {
		for(auto const& item : completionCanary) {
			if(!item.second) { return; }
		}
		// The attack step can be completed
		scheduleCompletionDelay();
	}
}

void AttackNode::notifyCompletion() {
	for(int i = 0; i < this->gateSize("out"); i++) {
		this->send(new omnetpp::cMessage("Notify completion", KIND_NOTIFY_COMPLETED), "out", i);
	}
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
					std::shared_ptr<ITransition> unconCon = std::make_shared<EventTransition>(
							new SendTcpConnectAtkFactory(atkController),
							opState,
							new cMessage("TCPCONNECT", MSGKIND_CONNECT),
							EventMatchType::Kind,
							SimTime(1, SIMTIME_S),
							nullptr,
							this
						);
					// Push transitions into canary
					completionCanary.insert({unconCon.get(), false});
					unconnectedTransitions.push_back(unconCon);
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
					std::shared_ptr<ITransition>atkOpRead = std::make_shared<EventTransition>(
							new SendMmsRequestFactory(cliController),
							atkOperatingState,
							new cMessage("SENDREAD", SEND_MMS_READ),
							EventMatchType::Kind,
							SimTime(cliController->par("sendReadInterval")),
							cliController->par("sendReadInterval").getExpression(),
							this
					);
					atkOperatingTransitions.push_back(atkOpRead);
					std::shared_ptr<ITransition> atkOpCmd = std::make_shared<EventTransition>(
							new SendMmsRequestFactory(cliController),
							atkOperatingState,
							new cMessage("SENDCOMMAND", SEND_MMS_COMMAND),
							EventMatchType::Kind,
							SimTime(cliController->par("sendCommandInterval")),
							cliController->par("sendCommandInterval").getExpression(),
							this
					);
					atkOperatingTransitions.push_back(atkOpCmd);
					// Push transitions into canary
					completionCanary.insert({atkOpRead.get(), false});
					completionCanary.insert({atkOpCmd.get(), false});
					atkOperatingState->setTransitions(atkOperatingTransitions);

					OpFSM* fsm = new OpFSM(controller, operatingState, false);

					cliController->getControlFSM()->merge(fsm);
					break;
				}
				case AttackType::WRITEOP: {
					MmsAttackerController* atkController = static_cast<MmsAttackerController*>(controller);
					OpState* opState = new OpState("OPERATIVE");

					std::vector<std::shared_ptr<ITransition>> operativeTransitions;
					std::shared_ptr<ITransition> opToCli = std::make_shared<PacketTransition>(
							new ForwardMmsMessageToClientFactory(atkController),
							opState,
							"content.messageKind == 1 || content.messageKind == 3", // messageKind == MMSKind::MEASURE || messageKind == MMSKind::GENRESP
							this
						);
					operativeTransitions.push_back(opToCli);
					std::shared_ptr<ITransition> opToSer = std::make_shared<PacketTransition>(
							new ForwardMmsMessageToServerFactory(atkController),
							opState,
							"content.messageKind == 0 || content.messageKind == 2", // messageKind == MMSKind::CONNECT || messageKind == MMSKind::GENREQ
							this
						);
					operativeTransitions.push_back(opToSer);
					// Push transitions into canary
					completionCanary.insert({opToCli.get(), false});
					completionCanary.insert({opToSer.get(), false});
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
	    emit(complTimeSignal, simTime() - this->stepStart);
	    emit(endTimeSignal, simTime());
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
