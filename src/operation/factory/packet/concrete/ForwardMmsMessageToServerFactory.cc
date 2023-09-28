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

#include "ForwardMmsMessageToServerFactory.h"
#include "../../../attacker/concrete/ForwardMmsMessageToServer.h"

using namespace inet;

void ForwardMmsMessageToServerFactory::build(Packet* packet) {
	MmsAttackerController* controller = check_and_cast<MmsAttackerController*>(this->controller);
    auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
    queue.push(chunk);
    while (queue.has<MmsMessage>(b(-1))) {
    	const auto& appmsg = queue.pop<MmsMessage>(b(-1));
    	MmsMessage* msg = controller->messageCopier->copyMessageNorm(appmsg.get(), true);

    	double p = controller->uniform(0.0, 1.0);
	    if(appmsg->getMessageKind() == MMSKind::GENREQ) {
			if(appmsg->getReqResKind() == ReqResKind::READ) {
				if (p < controller->readRequestBlockProb) { // Block
					controller->emit(controller->readRequestBlockSignal, true);
					msg->setAtkStatus(MITMKind::BLOCK);
					if(controller->isLogging) controller->logger->log(msg, EvilStateName::FULL, simTime());
					delete msg;
					return;
				} else if (p - controller->readRequestBlockProb < controller->readRequestCompromisedProb) { // Compromise
					msg->setAtkStatus(MITMKind::COMPR);
					msg->setData(9);
					if(controller->isLogging) controller->logger->log(msg, EvilStateName::FULL, simTime());
					controller->emit(controller->readRequestCompromisedSignal, true);
				} else {
					if(controller->isLogging) controller->logger->log(msg, EvilStateName::FULL, simTime());
				}
			} else if(appmsg->getReqResKind() == ReqResKind::COMMAND) {
				if (p < controller->commandRequestBlockProb) { // Block
					controller->emit(controller->commandRequestBlockSignal, true);
					msg->setAtkStatus(MITMKind::BLOCK);
					if(controller->isLogging) controller->logger->log(msg, EvilStateName::FULL, simTime());
					delete msg;
					return;
				} else if (p - controller->commandRequestBlockProb < controller->commandRequestCompromisedProb) { // Compromise
					controller->emit(controller->commandRequestCompromisedSignal, true);
					msg->setAtkStatus(MITMKind::COMPR);
					msg->setData(9);
					if(controller->isLogging) controller->logger->log(msg, EvilStateName::FULL, simTime());
				} else {
					if(controller->isLogging) controller->logger->log(msg, EvilStateName::FULL, simTime());
				}
			}
		}


		const auto& payload = controller->messageCopier->copyMessage(msg, true);
		delete msg;
		Packet *pckt = new Packet("data");
		pckt->insertAtBack(payload);
		ForwardMmsMessageToServer* msgToSer = new ForwardMmsMessageToServer(pckt);
		controller->enqueueNSchedule(msgToSer);
    }
}

ForwardMmsMessageToServerFactory::ForwardMmsMessageToServerFactory(MmsAttackerController* controller) {
	this->controller = controller;
}

ForwardMmsMessageToServerFactory::~ForwardMmsMessageToServerFactory() { }

