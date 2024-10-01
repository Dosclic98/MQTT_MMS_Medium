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

#ifndef OPERATION_ATTACKER_CONCRETE_MANAGEHTTPTCPSOCKETOPENCLOSEATK_H_
#define OPERATION_ATTACKER_CONCRETE_MANAGEHTTPTCPSOCKETOPENCLOSEATK_H_

#include "../HttpAttackerOperation.h"

namespace inet {

class ManageHttpTcpSocketOpenCloseAtk : public HttpAttackerOperation {
public:
    virtual void execute() override;

    ManageHttpTcpSocketOpenCloseAtk(bool open);
    virtual ~ManageHttpTcpSocketOpenCloseAtk();

protected:
    bool open;
};

}; // namespace inet

#endif /* OPERATION_ATTACKER_CONCRETE_MANAGEHTTPTCPSOCKETOPENCLOSEATK_H_ */
