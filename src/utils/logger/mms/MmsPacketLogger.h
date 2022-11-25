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
	MmsPacketLogger(std::string fileName);
	virtual ~MmsPacketLogger();

	std::string path = "simulations/logs/";

	void log(MmsMessage msg);
	void close();

protected:
	std::ofstream logFile;
};

};

#endif /* UTILS_LOGGER_MMS_MMSPACKETLOGGER_H_ */
