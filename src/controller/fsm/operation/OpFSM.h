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

#ifndef CONTROLLER_FSM_OPERATION_OPFSM_H_
#define CONTROLLER_FSM_OPERATION_OPFSM_H_

#include "../IFSM.h"
#include "../state/IState.h"

namespace inet {

class OpFSM : public IFSM {
public:
	virtual IState* getCurrentState() override;
	virtual void setCurrentState(IState* currentState) override;
	virtual IState* next(Packet* msg) override;
	virtual IState* next(cEvent* event) override;

	OpFSM(IController* owner, IState* currentState);
	virtual ~OpFSM() override;
};

} // namespace inet

#endif /* CONTROLLER_FSM_OPERATION_OPFSM_H_ */
