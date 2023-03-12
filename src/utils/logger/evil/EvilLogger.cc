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
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace inet {

EvilLogger::EvilLogger(int runNumber, int loggerIndex)
{
	this->loggerIndex = loggerIndex;

	// Create the correct folder structure if does not exists
	createFolderAtPath(this->path);
	std::string actualPath = this->path + "run" + std::to_string(runNumber) + "/";
	createFolderAtPath(actualPath);

	std::string fileName = "evilClient.app[" + std::to_string(loggerIndex) + "].csv";
	logFile.open(actualPath + fileName);
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

void EvilLogger::createFolderAtPath(std::string& path) {
	fs::create_directory(path); // create folder at path
}

EvilLogger::~EvilLogger() {
	logFile.close();
}

};
