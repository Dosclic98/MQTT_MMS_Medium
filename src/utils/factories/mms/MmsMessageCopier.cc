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

#include "MmsMessageCopier.h"

#include "inet/common/TimeTag_m.h"
#include "inet/common/socket/SocketTag_m.h"

namespace inet {

MmsMessageCopier::MmsMessageCopier() {

}

MmsMessageCopier::~MmsMessageCopier()
{
	// TODO Auto-generated destructor stub
}

const Ptr<MmsMessage> MmsMessageCopier::copyMessage(const MmsMessage* appmsg, int connId, int evilServerConnId, bool fakeCreatTime) {
	const auto& msg = makeShared<MmsMessage>();
	msg->setOriginId(appmsg->getOriginId());
	msg->setMessageKind(appmsg->getMessageKind());
	msg->setReqResKind(appmsg->getReqResKind());
	msg->setConnId(connId);
	msg->setExpectedReplyLength(appmsg->getExpectedReplyLength());
	msg->setChunkLength(appmsg->getChunkLength());
	msg->setEvilServerConnId(evilServerConnId);
	msg->setServerClose(appmsg->getServerClose());
	msg->setData(appmsg->getData());
	msg->setAtkStatus(appmsg->getAtkStatus());
	if(fakeCreatTime) msg->addTag<CreationTimeTag>()->setCreationTime(appmsg->getTag<CreationTimeTag>()->getCreationTime());
	else msg->addTag<CreationTimeTag>()->setCreationTime(simTime());
	msg->setServerIndex(appmsg->getServerIndex());

	return msg;
}


const Ptr<MmsMessage> MmsMessageCopier::copyMessage(const MmsMessage* appmsg, int connId, bool fakeCreatTime) {
	return copyMessage(appmsg, connId, appmsg->getEvilServerConnId(), fakeCreatTime);
}

const Ptr<MmsMessage> MmsMessageCopier::copyMessage(const MmsMessage* appmsg, bool fakeCreatTime) {
	return this->copyMessage(appmsg, appmsg->getConnId(), fakeCreatTime);
}

MmsMessage* MmsMessageCopier::copyMessageNorm(const MmsMessage* appmsg, int connId, int evilServerConnId, bool fakeCreatTime) {
	MmsMessage* msg = new MmsMessage();
	msg->setOriginId(appmsg->getOriginId());
	msg->setMessageKind(appmsg->getMessageKind());
	msg->setReqResKind(appmsg->getReqResKind());
	msg->setConnId(connId);
	msg->setExpectedReplyLength(appmsg->getExpectedReplyLength());
	msg->setChunkLength(appmsg->getChunkLength());
	msg->setEvilServerConnId(evilServerConnId);
	msg->setServerClose(appmsg->getServerClose());
	msg->setData(appmsg->getData());
	msg->setAtkStatus(appmsg->getAtkStatus());
	if(fakeCreatTime) msg->addTag<CreationTimeTag>()->setCreationTime(appmsg->getTag<CreationTimeTag>()->getCreationTime());
	else msg->addTag<CreationTimeTag>()->setCreationTime(simTime());
	msg->setServerIndex(appmsg->getServerIndex());

	return msg;
}

MmsMessage* MmsMessageCopier::copyMessageNorm(const MmsMessage* appmsg, int connId, bool fakeCreatTime) {
	return copyMessageNorm(appmsg, connId, appmsg->getEvilServerConnId(), fakeCreatTime);
}

MmsMessage* MmsMessageCopier::copyMessageNorm(const MmsMessage* appmsg, bool fakeCreatTime) {
	return this->copyMessageNorm(appmsg, appmsg->getConnId(), fakeCreatTime);
}

};

