/*
 * EvilLogger.cc
 *
 *  Created on: Jan 23, 2023
 *      Author: dosclic98
 */

#include "EvilLogger.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/socket/SocketTag_m.h"
#include <stdio.h>
#include <stdlib.h>

namespace inet {

EvilLogger::EvilLogger()
{
	std::string fileName = "evilClient.csv";
	logFile.open(this->path + fileName);
	logFile << "id,messageKind,reqResKind,atkStatus,data,evilState,creationTime,timestamp\n";
}

void EvilLogger::log(const MmsMessage* msg, EvilStateName evilState, simtime_t timestamp) {
	std::string mmsKindStr = "";
	std::string reqResKindStr = "";
	std::string mitmKindStr = mitmKindToStr[msg->getAtkStatus()];
	if(msg->getMessageKind() == MMSKind::FAKE) { mmsKindStr = "FAKE"; }
	else mmsKindStr = mmsKindToStr[msg->getMessageKind()];
	if(msg->getReqResKind() == ReqResKind::UNSET) { reqResKindStr = "UNSET"; }
	else reqResKindStr = reqResKindToStr[msg->getReqResKind()];
	logFile << msg->getOriginId() << "," << mmsKindStr << "," << reqResKindStr << "," << mitmKindStr << "," << msg->getData()
			<< "," << EvilState::stateNames[evilState] << "," << msg->getTag<CreationTimeTag>()->getCreationTime() << "," << timestamp << "\n";
}

EvilLogger::~EvilLogger() {
	logFile.close();
}

};
