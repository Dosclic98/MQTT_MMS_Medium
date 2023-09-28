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

#include "ForwardMmsMessageToClientFactory.h"
#include "inet/common/socket/SocketTag_m.h"

#include "../../../attacker/concrete/ForwardMmsMessageToClient.h"

using namespace inet;

void ForwardMmsMessageToClientFactory::build(Packet* packet) {
	MmsAttackerController* controller = check_and_cast<MmsAttackerController*>(this->controller);
    auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
    queue.push(chunk);
    while (queue.has<MmsMessage>(b(-1))) {
    	const auto& appmsg = queue.pop<MmsMessage>(b(-1));
		MmsMessage* msg = controller->messageCopier->copyMessageNorm(appmsg.get(), true);
		Packet *pckt = new Packet("data");

		MMSKind messageKind = appmsg->getMessageKind();
		ReqResKind reqResKind = appmsg->getReqResKind();

	    double p = controller->uniform(0.0, 1.0);
	    if (messageKind == MMSKind::MEASURE) {
	        if (p < controller->measureBlockProb) { //Block
	        	controller->bubble("Measure blocked");
	        	controller->emit(controller->measureBlockSignal, true);
	            msg->setAtkStatus(MITMKind::BLOCK);
	            if(controller->isLogging) controller->logger->log(msg, EvilStateName::FULL, simTime());
	            delete msg;
	            delete pckt;
	            return;
	        } else if (p - controller->measureBlockProb < controller->measureCompromisedProb) { //Compromise
	        	controller->bubble("Measure compromised");
	        	controller->emit(controller->measureCompromisedSignal, true);
	            msg->setAtkStatus(MITMKind::COMPR);
	            msg->setData(9);
	            if(controller->isLogging) controller->logger->log(msg, EvilStateName::FULL, simTime());
	        } else {
	        	controller->bubble("Measure arrived from server");
	        	if(controller->isLogging) controller->logger->log(msg, EvilStateName::FULL, simTime());
	        }
	    } else if (messageKind == MMSKind::GENRESP) {
	    	if(reqResKind == ReqResKind::READ) {
		        if (p < controller->readResponseBlockProb) { // Block
		        	controller->bubble("Read response blocked");
		        	controller->emit(controller->readResponseBlockSignal, true);
		            msg->setAtkStatus(MITMKind::BLOCK);
		            if(controller->isLogging) controller->logger->log(msg, EvilStateName::FULL, simTime());
		            delete msg;
		            delete pckt;
		            return;
		        } else if (p - controller->readResponseBlockProb < controller->readResponseCompromisedProb) { // Compromise
		        	controller->bubble("Read response compromised");
		        	controller->emit(controller->readResponseCompromisedSignal, true);
		            msg->setAtkStatus(MITMKind::COMPR);
		            msg->setData(9);
		            if(controller->isLogging) controller->logger->log(msg, EvilStateName::FULL, simTime());
		        } else {
		        	controller->bubble("Read response arrived from server");
		        	if(controller->isLogging) controller->logger->log(msg, EvilStateName::FULL, simTime());
		        }
	    	} else if(reqResKind == ReqResKind::COMMAND) {
		        if (p < controller->commandResponseBlockProb) { // Block
		        	controller->bubble("Command response blocked");
		        	controller->emit(controller->commandResponseBlockSignal, true);
		            msg->setAtkStatus(MITMKind::BLOCK);
		            if(controller->isLogging) controller->logger->log(msg, EvilStateName::FULL, simTime());
		            delete msg;
		            delete pckt;
		            return;
		        } else if (p - controller->commandResponseBlockProb < controller->commandResponseCompromisedProb) { // Compromise
		        	controller->bubble("Command response compromised");
		        	controller->emit(controller->commandResponseCompromisedSignal, true);
		            msg->setAtkStatus(MITMKind::COMPR);
		            msg->setData(9);
		            if(controller->isLogging) controller->logger->log(msg, EvilStateName::FULL, simTime());
		        } else {
		        	controller->bubble("Command response arrived from server");
		        	if(controller->isLogging) controller->logger->log(msg, EvilStateName::FULL, simTime());
		        }
	    	}
	    }

    	const auto& msgCp = controller->messageCopier->copyMessage(msg, msg->getEvilServerConnId(), true);
    	pckt->insertAtBack(msgCp);
    	pckt->addTag<SocketInd>()->setSocketId(appmsg->getEvilServerConnId());

    	ForwardMmsMessageToClient* cliOp = new ForwardMmsMessageToClient(pckt);
	    // Propagate immediatly and do not schedule it in the queue
    	// to not introduce additional delay
    	controller->propagate(cliOp);
    }
}

ForwardMmsMessageToClientFactory::ForwardMmsMessageToClientFactory(MmsAttackerController* controller) {
	this->controller = controller;
}

ForwardMmsMessageToClientFactory::~ForwardMmsMessageToClientFactory() { }

