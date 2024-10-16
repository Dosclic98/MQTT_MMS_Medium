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

#ifndef OPERATION_CLIENT_CONCRETE_SENDMMSDISCONNECT_H_
#define OPERATION_CLIENT_CONCRETE_SENDMMSDISCONNECT_H_

#include "../MmsClientOperation.h"


namespace inet {

class SendMmsDisconnect: public MmsClientOperation {
public:
	SendMmsDisconnect();
	virtual ~SendMmsDisconnect();

	virtual void execute() override;
};

} // namespace inet

#endif /* OPERATION_CLIENT_CONCRETE_SENDMMSDISCONNECT_H_ */
