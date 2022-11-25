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

#include "MmsPacketLogger.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/socket/SocketTag_m.h"
#include <stdio.h>
#include <stdlib.h>

namespace inet {

MmsPacketLogger::MmsPacketLogger(int numClient, int numApp) {
	std::string fileName = "client[" + std::to_string(numClient) + "].app[" + std::to_string(numApp) + "].csv";
	logFile.open(this->path + fileName);
	logFile << "id,messageKind,reqResKind,creationTime,timestamp\n";
}

MmsPacketLogger::~MmsPacketLogger() {
	logFile.close();
}

void MmsPacketLogger::log(MmsMessage* msg, simtime_t timestamp) {
	std::string mmsKindStr = "";
	std::string reqResKindStr = "";
	if(msg->getMessageKind() == MMSKind::FAKE) { mmsKindStr = "FAKE"; }
	else mmsKindStr = mmsKindToStr[msg->getMessageKind()];
	if(msg->getReqResKind() == ReqResKind::UNSET) { reqResKindStr = "UNSET"; }
	else reqResKindStr = reqResKindToStr[msg->getReqResKind()];
	logFile << msg->getOriginId() << "," << mmsKindStr << "," << reqResKindStr << "," << msg->getTag<CreationTimeTag>()->getCreationTime() << "," << timestamp << "\n";
}

};

