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
#include "../../controller/attacker/HttpAttackerController.h"
#include "../../controller/attacker/MmsAttackerController.h"
#include "../../controller/fsm/concrete/OpFSM.h"
#include "../../controller/fsm/state/concrete/OpState.h"
#include "../../controller/fsm/transition/concrete/EventTransition.h"
#include "../../controller/fsm/transition/concrete/PacketTransition.h"
#include "../../operation/factory/event/concrete/GenHttpTcpConnectTimeoutFactory.h"
#include "../../operation/factory/packet/concrete/ForwardMmsMessageToServerFactory.h"
#include "../../operation/factory/packet/concrete/ForwardMmsMessageToClientFactory.h"
#include "../../operation/factory/event/concrete/SendTcpConnectAtkFactory.h"
#include "../../operation/factory/event/concrete/SendHttpRequestFactory.h"
#include "../../operation/factory/event/concrete/SendHttpRequestBruteforceFactory.h"
#include "../../operation/factory/event/concrete/PlaceholderEventOperationFactory.h"
#include "../../operation/factory/event/concrete/SendHttpTcpConnectFactory.h"
#include "../../operation/factory/event/concrete/SendHttpTcpDisconnectFactory.h"
#include "../../operation/factory/packet/concrete/PlaceholderPacketOperationFactory.h"
#include "../../operation/factory/event/concrete/SendTcpConnectFactory.h"
#include "../../operation/factory/event/concrete/SendMmsDisconnectFactory.h"
#include "../../operation/factory/event/concrete/SendMmsConnectFactory.h"
#include "../../operation/factory/event/concrete/SendMmsRequestFactory.h"
#include "../../operation/factory/packet/concrete/ManageHttpTcpSocketFactory.h"
#include "../../utils/factories/http/HttpMessageFactory.h"

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
    				// If the node do not posess a CFSM (no target controller and empty canary)
    				// just schedule the completion event for the step
    				if(this->getNodeType() == NodeType::AND || this->getNodeType() == NodeType::OR ||
    				        (this->getNodeType() == NodeType::STEP && this->targetControllers.empty() &&
    				                this->completionCanary.empty())) {
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
    HttpMessageFactory httpFactory = HttpMessageFactory();

	if(this->nodeType == NodeType::STEP) {
		for(IController* controller : this->targetControllers) {
		    switch(this->attackType) {
		        case AttackType::SCANIP: {
		            HttpAttackerController* atkController = static_cast<HttpAttackerController*>(controller);
		            OpState* scanningState = new OpState("SCANNING");
		            OpState* connectingState = new OpState("CONNECTING");
		            OpState* connectedState = new OpState("CONNECTED");
		            OpState* disconnectingState = new OpState("DISCONNECTING");
		            OpState* doneState = new OpState("DONE");

		            std::vector<std::shared_ptr<ITransition>> scanningTransitions;
		            std::shared_ptr<ITransition> scanConning = std::make_shared<EventTransition>(
		                    new SendHttpTcpConnectFactory(atkController),
		                    connectingState,
		                    atkController->connectionTimer,
		                    EventMatchType::Ref,
		                    SimTime(20, SIMTIME_MS));
		            std::shared_ptr<ITransition> scanDone = std::make_shared<EventTransition>(
		                    new PlaceholderEventOperationFactory(atkController),
		                    doneState,
		                    atkController->ipsFinishedTimer,
		                    EventMatchType::Ref,
		                    SimTime(1, SIMTIME_MS),
		                    nullptr,
		                    this,
		                    true);
		            // Push scanning --> done transition into canary
		            completionCanary.insert({scanDone.get(), false});
		            scanningTransitions.push_back(scanConning);
		            scanningTransitions.push_back(scanDone);
		            scanningState->setTransitions(scanningTransitions);

		            std::vector<std::shared_ptr<ITransition>> connectingTransitions;
		            std::shared_ptr<ITransition> conScanning = std::make_shared<EventTransition>(
		                    new GenHttpTcpConnectTimeoutFactory(atkController),
		                    scanningState,
		                    atkController->timeoutTimer,
		                    EventMatchType::Ref,
		                    atkController->connectTimeout);
		            std::shared_ptr<ITransition> conConnected = std::make_shared<PacketTransition>(
		                    new ManageHttpTcpSocketFactory(atkController),
		                    connectedState,
		                    "kind == 1");
		            std::shared_ptr<ITransition> conScanningRst = std::make_shared<PacketTransition>(
		                    new ManageHttpTcpSocketFactory(atkController),
		                    scanningState,
		                    "kind == 3");
		            connectingTransitions.push_back(conScanning);
		            connectingTransitions.push_back(conConnected);
		            connectingTransitions.push_back(conScanningRst);
		            connectingState->setTransitions(connectingTransitions);

		            std::vector<std::shared_ptr<ITransition>> connectedTransitions;
		            std::shared_ptr<ITransition> conDisc = std::make_shared<EventTransition>(
		                    new SendHttpTcpDisconnectFactory(atkController),
		                    disconnectingState,
		                    atkController->disconnectionTimer,
		                    EventMatchType::Ref,
		                    SimTime(20, SIMTIME_MS));
		            connectedTransitions.push_back(conDisc);
		            connectedState->setTransitions(connectedTransitions);

		            std::vector<std::shared_ptr<ITransition>> disconnectingTransitions;
		            std::shared_ptr<ITransition> disScanning = std::make_shared<PacketTransition>(
                            new ManageHttpTcpSocketFactory(atkController),
                            scanningState,
                            "kind == 3");
		            disconnectingTransitions.push_back(disScanning);
		            disconnectingState->setTransitions(disconnectingTransitions);


		            OpFSM* fsm = new OpFSM(controller, scanningState, false);
		            // After all IPs get scanned we can disable the
		            // and enable the scanDone scanConning transition
		            fsm->addDormancyUpdate(atkController->ipsFinishedTimer, scanConning, true);
		            fsm->addDormancyUpdate(atkController->ipsFinishedTimer, scanDone, false);
		            atkController->getControlFSM()->merge(fsm);
		            break;
		        }
		        case AttackType::SCANVULN: {
		            HttpAttackerController* atkController = static_cast<HttpAttackerController*>(controller);
		            OpState* doneState = new OpState("DONE");
		            OpState* startingState = new OpState("STARTING-VULN");
		            OpState* connectingState = new OpState("CONNECTING-VULN");
		            OpState* connectedState = new OpState("CONNECTED-VULN");
		            OpState* waitHttpRespState = new OpState("WAITFORHTTPRESP-VULN");
		            OpState* failState = new OpState("FAIL-VULN");
		            OpState* doneVulnState = new OpState("DONE-VULN");

		            std::vector<std::shared_ptr<ITransition>> doneTransitions;
		            std::shared_ptr<ITransition> scanDone = std::make_shared<EventTransition>(
                            new PlaceholderEventOperationFactory(atkController),
                            startingState,
                            atkController->startingTimer,
                            EventMatchType::Ref,
                            SimTime(1, SIMTIME_MS));
		            doneTransitions.push_back(scanDone);
		            doneState->setTransitions(doneTransitions);

		            std::vector<std::shared_ptr<ITransition>> startingTransitions;
		            std::shared_ptr<ITransition> startConning = std::make_shared<EventTransition>(
		                    new SendHttpTcpConnectFactory(atkController),
                            connectingState,
                            atkController->connectionTimer,
                            EventMatchType::Ref,
                            SimTime(20, SIMTIME_MS));
		            startingTransitions.push_back(startConning);
		            startingState->setTransitions(startingTransitions);

		            std::vector<std::shared_ptr<ITransition>> connectingTransitions;
		            std::shared_ptr<ITransition> conStarting = std::make_shared<EventTransition>(
                            new GenHttpTcpConnectTimeoutFactory(atkController),
                            startingState,
                            atkController->timeoutTimer,
                            EventMatchType::Ref,
                            atkController->connectTimeout);
		            std::shared_ptr<ITransition> conStartingRst = std::make_shared<PacketTransition>(
                            new ManageHttpTcpSocketFactory(atkController),
                            startingState,
                            "kind == 3");
		            std::shared_ptr<ITransition> conConnected = std::make_shared<PacketTransition>(
                            new ManageHttpTcpSocketFactory(atkController),
                            connectedState,
                            "kind == 1");
		            connectingTransitions.push_back(conStarting);
		            connectingTransitions.push_back(conStartingRst);
		            connectingTransitions.push_back(conConnected);
		            connectingState->setTransitions(connectingTransitions);

		            std::vector<std::shared_ptr<ITransition>> connectedTransitions;
		            std::shared_ptr<ITransition> conResWait = std::make_shared<EventTransition>(
		                    new SendHttpRequestFactory(atkController, httpFactory.buildRequest("GET", "/api/info")),
		                    waitHttpRespState,
		                    atkController->sendRequestTimer,
		                    EventMatchType::Ref,
		                    SimTime(20, SIMTIME_MS));
		            connectedTransitions.push_back(conResWait);
		            connectedState->setTransitions(connectedTransitions);

		            std::vector<std::shared_ptr<ITransition>> waitingTransitions;
		            std::shared_ptr<ITransition> waitingDone = std::make_shared<PacketTransition>(
		                    new PlaceholderPacketOperationFactory(atkController),
		                    doneVulnState,
		                    "content.result == 200",
		                    this);
		            std::shared_ptr<ITransition> waitingFail = std::make_shared<PacketTransition>(
                            new PlaceholderPacketOperationFactory(atkController),
                            failState,
                            "content.result != 200");
		            // Push waitingResponse --> done transition into canary
		            completionCanary.insert({waitingDone.get(), false});
		            waitingTransitions.push_back(waitingDone);
		            waitingTransitions.push_back(waitingFail);
		            waitHttpRespState->setTransitions(waitingTransitions);

		            std::vector<std::shared_ptr<ITransition>> failTransitions;
		            std::shared_ptr<ITransition> failScanning = std::make_shared<EventTransition>(
		                    new SendHttpTcpDisconnectFactory(atkController, false),
		                    startingState,
		                    atkController->timeoutTimer,
		                    EventMatchType::Ref,
		                    SimTime(0, SIMTIME_MS));
		            failTransitions.push_back(failScanning);
		            failState->setTransitions(failTransitions);

		            OpFSM* fsm = new OpFSM(controller, doneState, false);
                    atkController->getControlFSM()->merge(fsm);
                    break;
		        }
		        case AttackType::BRUTEFORCE: {
		            HttpClientController* cliController = static_cast<HttpClientController*>(controller);
		            OpState* startingState = new OpState("STARTING");
		            OpState* connectingState = new OpState("CONNECTING");
		            OpState* connectedState = new OpState("CONNECTED");
		            OpState* waitHttpRespState = new OpState("WAITFORHTTPRESP");
		            OpState* doneState = new OpState("DONE");

		            std::vector<std::shared_ptr<ITransition>> startingTransitions;
		            std::shared_ptr<ITransition> startConning = std::make_shared<EventTransition>(
		                    new SendHttpTcpConnectFactory(cliController),
                            connectingState,
                            cliController->startingTimer,
                            EventMatchType::Ref,
                            SimTime(20, SIMTIME_MS));
		            startingTransitions.push_back(startConning);
		            startingState->setTransitions(startingTransitions);

		            std::vector<std::shared_ptr<ITransition>> connectingTransitions;
                    std::shared_ptr<ITransition> conStarting = std::make_shared<EventTransition>(
                            new GenHttpTcpConnectTimeoutFactory(cliController),
                            startingState,
                            cliController->timeoutTimer,
                            EventMatchType::Ref,
                            cliController->connectTimeout);
                    std::shared_ptr<ITransition> conStartingRst = std::make_shared<PacketTransition>(
                            new ManageHttpTcpSocketFactory(cliController),
                            startingState,
                            "kind == 3");
                    std::shared_ptr<ITransition> conConnected = std::make_shared<PacketTransition>(
                            new ManageHttpTcpSocketFactory(cliController),
                            connectedState,
                            "kind == 1");

                    connectingTransitions.push_back(conStarting);
                    connectingTransitions.push_back(conStartingRst);
                    connectingTransitions.push_back(conConnected);
                    connectingState->setTransitions(connectingTransitions);

                    std::vector<std::shared_ptr<ITransition>> connectedTransitions;
                    // TODO Revise the delay expression system using type const char* and parsing
                    // the expression within the transition
                    std::shared_ptr<ITransition> conResWait = std::make_shared<EventTransition>(
                            new SendHttpRequestBruteforceFactory(cliController),
                            waitHttpRespState,
                            cliController->sendRequestTimer,
                            EventMatchType::Ref,
                            SimTime(1, SIMTIME_S));
                    connectedTransitions.push_back(conResWait);
                    connectedState->setTransitions(connectedTransitions);

                    std::vector<std::shared_ptr<ITransition>> waitingTransitions;
                    std::shared_ptr<ITransition> waitingDone = std::make_shared<PacketTransition>(
                            new PlaceholderPacketOperationFactory(cliController),
                            doneState,
                            "content.result == 200",
                            this);
                    std::shared_ptr<ITransition> waitingConnected = std::make_shared<PacketTransition>(
                            new PlaceholderPacketOperationFactory(cliController),
                            connectedState,
                            "content.result != 200");

                    // Push waitingResponse --> done transition into canary
                    completionCanary.insert({waitingDone.get(), false});
                    waitingTransitions.push_back(waitingDone);
                    waitingTransitions.push_back(waitingConnected);
                    waitHttpRespState->setTransitions(waitingTransitions);

                    OpFSM* fsm = new OpFSM(controller, startingState, false);
                    cliController->getControlFSM()->merge(fsm);
                    break;
		        }
		        case AttackType::CREDACC: {
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
		        case AttackType::AITM: {
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
                            SimTime(200, SIMTIME_MS)
                    ));
                    operatingState->setTransitions(operatingTransitions);


                    std::vector<std::shared_ptr<ITransition>> terminatedTransitions;
                    std::string* strConnAddr = new std::string("IncrementalTest.workstation.attackerOperator");
                    terminatedTransitions.push_back(std::make_shared<EventTransition>(
                            new SendTcpConnectFactory(cliController, strConnAddr),
                            atkConnectedState,
                            new cMessage("TCPCONNECT", SEND_TCP_CONNECT),
                            EventMatchType::Kind,
                            SimTime(200, SIMTIME_MS)
                    ));
                    terminatedState->setTransitions(terminatedTransitions);

                    // Create connected transitions
                    std::vector<std::shared_ptr<ITransition>> atkConnectedTransitions;
                    atkConnectedTransitions.push_back(std::make_shared<EventTransition>(
                            new SendMmsConnectFactory(cliController),
                            atkOperatingState,
                            new cMessage("SENDMEAS", SEND_MMS_CONNECT),
                            EventMatchType::Kind,
                            SimTime(20, SIMTIME_MS)
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
		        case AttackType::UNAUTHCMDMSG: {
                    MmsAttackerController* atkController = static_cast<MmsAttackerController*>(controller);
                    OpState* opState = new OpState("OPERATIVE");

                    std::vector<std::shared_ptr<ITransition>> operativeTransitions;
                    std::shared_ptr<ITransition> opToSer = std::make_shared<PacketTransition>(
                            new ForwardMmsMessageToServerFactory(atkController),
                            opState,
                            "content.messageKind == 0 || content.messageKind == 2", // messageKind == MMSKind::CONNECT || messageKind == MMSKind::GENREQ
                            this
                        );
                    operativeTransitions.push_back(opToSer);
                    // Push transition into canary
                    completionCanary.insert({opToSer.get(), false});
                    opState->setTransitions(operativeTransitions);
                    OpFSM* fsm = new OpFSM(controller, opState, false);
                    atkController->getControlFSM()->merge(fsm);
		            break;
		        }
		        case AttackType::SPOOFREPMSG: {
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
                    // Push transition into canary
                    completionCanary.insert({opToCli.get(), false});
                    opState->setTransitions(operativeTransitions);
                    OpFSM* fsm = new OpFSM(controller, opState, false);
                    atkController->getControlFSM()->merge(fsm);
		        }
		        default:
		            EV_INFO << "No action defined for the activated attack step\n";
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
