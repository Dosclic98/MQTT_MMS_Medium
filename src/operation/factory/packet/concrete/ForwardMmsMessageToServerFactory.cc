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
#include "../../../../controller/fsm/state/IState.h"

using namespace inet;

void ForwardMmsMessageToServerFactory::build(Packet* packet) {
	MmsAttackerController* controller = check_and_cast<MmsAttackerController*>(this->controller);
    auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
    queue.push(chunk);
    while (queue.has<MmsMessage>(b(-1))) {
    	const auto& appmsg = queue.pop<MmsMessage>(b(-1));

    	// Update the numGenReq counter and generate a fake one if necessary
		if(appmsg->getMessageKind() == MMSKind::GENREQ) {
			controller->setNumGenReq(controller->getNumGenReq() + 1);
			// If at least a certain number of generic requests has been sent
			// generate a fake request for the server
			if(controller->getNumGenReq() >= controller->getFakeGenReqThresh()) {
				controller->setNumGenReq(0);

				// Add to the forward to the server queue
				MmsMessage* msg = new MmsMessage();
				// Useless, could also not be set
				msg->setOriginId(packet->getId());
				msg->setMessageKind(MMSKind::GENREQ);
				// Set to read for now
				msg->setReqResKind(ReqResKind::READ);
				msg->setConnId(appmsg->getConnId());
				msg->setExpectedReplyLength(appmsg->getExpectedReplyLength());
				msg->setChunkLength(appmsg->getChunkLength());
				// Set to -1 so the ClientEvilComp Will not forward it to the server
				msg->setEvilServerConnId(-1);
				msg->setServerClose(false);
				msg->addTag<CreationTimeTag>()->setCreationTime(simTime());
				msg->setData(0);
				msg->setAtkStatus(MITMKind::FAKEGEN);
				if(controller->isAtkLogging()) controller->log(appmsg.get(), controller->getControlFSM()->getCurrentState()->getName(), simTime());

				const auto& payload = controller->messageCopier->copyMessage(msg, true);
				Packet *pckt = new Packet("data");
				pckt->insertAtBack(payload);
				ForwardMmsMessageToServer* fakeMsgToSer = new ForwardMmsMessageToServer(pckt);

				EV << "Sending Fake message\n";
				controller->enqueueNSchedule(fakeMsgToSer);
				delete msg;
			}
		}

    	MmsMessage* msg = controller->messageCopier->copyMessageNorm(appmsg.get(), true);
    	double p = controller->uniform(0.0, 1.0);
	    if(appmsg->getMessageKind() == MMSKind::GENREQ) {
			if(appmsg->getReqResKind() == ReqResKind::READ) {
				if (p < controller->readRequestBlockProb) { // Block
					controller->emit(controller->readRequestBlockSignal, true);
					msg->setAtkStatus(MITMKind::BLOCK);
					if(controller->isAtkLogging()) controller->log(appmsg.get(), controller->getControlFSM()->getCurrentState()->getName(), simTime());
					delete msg;
					return;
				} else if (p - controller->readRequestBlockProb < controller->readRequestCompromisedProb) { // Compromise
					msg->setAtkStatus(MITMKind::COMPR);
					msg->setData(9);
					if(controller->isAtkLogging()) controller->log(appmsg.get(), controller->getControlFSM()->getCurrentState()->getName(), simTime());
					controller->emit(controller->readRequestCompromisedSignal, true);
				} else {
					if(controller->isAtkLogging()) controller->log(appmsg.get(), controller->getControlFSM()->getCurrentState()->getName(), simTime());
				}
			} else if(appmsg->getReqResKind() == ReqResKind::COMMAND) {
				if (p < controller->commandRequestBlockProb) { // Block
					controller->emit(controller->commandRequestBlockSignal, true);
					msg->setAtkStatus(MITMKind::BLOCK);
					if(controller->isAtkLogging()) controller->log(appmsg.get(), controller->getControlFSM()->getCurrentState()->getName(), simTime());
					delete msg;
					return;
				} else if (p - controller->commandRequestBlockProb < controller->commandRequestCompromisedProb) { // Compromise
					controller->emit(controller->commandRequestCompromisedSignal, true);
					msg->setAtkStatus(MITMKind::COMPR);
					msg->setData(9);
					if(controller->isAtkLogging()) controller->log(appmsg.get(), controller->getControlFSM()->getCurrentState()->getName(), simTime());
				} else {
					if(controller->isAtkLogging()) controller->log(appmsg.get(), controller->getControlFSM()->getCurrentState()->getName(), simTime());
				}
			}
		}
	    if(appmsg->getMessageKind() == MMSKind::CONNECT) {
	    	if(controller->isAtkLogging()) controller->log(appmsg.get(), controller->getControlFSM()->getCurrentState()->getName(), simTime());
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

