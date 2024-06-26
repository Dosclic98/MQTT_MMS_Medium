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

#include "MmsServerPacketLogger.h"
#include "inet/common/TimeTag_m.h"

namespace inet {

MmsServerPacketLogger::MmsServerPacketLogger(int runNumber, std::string whatCompStr, int num, int numApp) {
	// Create the correct folder structure if does not exists
	createFolderAtPath(this->path);
	std::string actualPath = this->path + "run" + std::to_string(runNumber) + "/";
	createFolderAtPath(actualPath);

	std::string fileName = whatCompStr + "[" + std::to_string(num) + "].app[" + std::to_string(numApp) + "].csv";
	logFile.open(actualPath + fileName);
	logFile << "id,messageKind,reqResKind,atkStatus,data,serverOp,creationTime,timestamp\n";
}

void MmsServerPacketLogger::log(const MmsMessage* msg, int unstableStatus, simtime_t timestamp) {
	std::string mmsKindStr = "";
	std::string reqResKindStr = "";
	std::string mitmKindStr = mitmKindToStr[msg->getAtkStatus()];
	if(msg->getMessageKind() == MMSKind::FAKE) { mmsKindStr = "FAKE"; }
	else mmsKindStr = mmsKindToStr[msg->getMessageKind()];
	if(msg->getReqResKind() == ReqResKind::UNSET) { reqResKindStr = "UNSET"; }
	else reqResKindStr = reqResKindToStr[msg->getReqResKind()];
	logFile << msg->getOriginId() << "," << mmsKindStr << "," << reqResKindStr << "," << mitmKindStr << "," << msg->getData()
			<< "," << serverStatusStr[unstableStatus] << "," << msg->getTag<CreationTimeTag>()->getCreationTime() << "," << timestamp << "\n";
}

};

