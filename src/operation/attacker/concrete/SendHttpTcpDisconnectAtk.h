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

#ifndef OPERATION_ATTACKER_CONCRETE_SENDHTTPTCPDISCONNECTATK_H_
#define OPERATION_ATTACKER_CONCRETE_SENDHTTPTCPDISCONNECTATK_H_

#include "../HttpAttackerOperation.h"

namespace inet {

class SendHttpTcpDisconnectAtk : public HttpAttackerOperation {
public:
    SendHttpTcpDisconnectAtk();
    virtual ~SendHttpTcpDisconnectAtk();

    virtual void execute() override;
};

}; // namespace inet

#endif /* OPERATION_ATTACKER_CONCRETE_SENDHTTPTCPDISCONNECTATK_H_ */
