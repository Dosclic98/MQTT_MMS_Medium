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

#ifndef UTILS_LOGGER_MMS_MMSPACKETLOGGER_H_
#define UTILS_LOGGER_MMS_MMSPACKETLOGGER_H_

#include <iostream>
#include <fstream>
#include "../../../message/mms/MmsMessage_m.h"

namespace inet {

class MmsPacketLogger {
public:
	MmsPacketLogger(int numClient, int numApp);
	virtual ~MmsPacketLogger();

	std::string mmsKindToStr[5] = {"CONNECT", "MEASURE", "GENREQ", "GENRESP"};
	std::string reqResKindToStr[3] = {"READ", "COMMAND"};
	std::string path = "logs/";

	void log(MmsMessage* msg, simtime_t timestamp);

protected:
	std::ofstream logFile;
};

};

#endif /* UTILS_LOGGER_MMS_MMSPACKETLOGGER_H_ */
