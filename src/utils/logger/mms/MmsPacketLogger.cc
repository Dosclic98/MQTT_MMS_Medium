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

namespace inet {

MmsPacketLogger::MmsPacketLogger(std::string fileName) {
	logFile.open(this->path + fileName);
	logFile << "id,messageKind,reqResKind,creationTime,timestampn\n";
}

MmsPacketLogger::~MmsPacketLogger()
{
	// TODO Auto-generated destructor stub
}

void MmsPacketLogger::log(MmsMessage msg) {

}

void MmsPacketLogger::close() {

}

};

