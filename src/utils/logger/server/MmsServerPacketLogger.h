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

#ifndef UTILS_LOGGER_SERVER_MMSSERVERPACKETLOGGER_H_
#define UTILS_LOGGER_SERVER_MMSSERVERPACKETLOGGER_H_

#include "../mms/MmsPacketLogger.h"

namespace inet {

class MmsServerPacketLogger : public MmsPacketLogger
{
	public:
		std::string serverStatusStr[2] = {"STABLE", "UNSTABLE"};
		MmsServerPacketLogger(int runNumber, std::string whatCompStr, int num, int numApp);
		void log(const MmsMessage* msg, int unstableStatus, simtime_t timestamp);
};

};

#endif /* UTILS_LOGGER_SERVER_MMSSERVERPACKETLOGGER_H_ */
