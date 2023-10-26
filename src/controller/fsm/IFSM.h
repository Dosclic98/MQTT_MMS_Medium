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

#ifndef CONTROLLER_FSM_IFSM_H_
#define CONTROLLER_FSM_IFSM_H_

#include "inet/common/packet/Packet.h"

namespace inet {

class IState;
class IController;

class IFSM {
protected:
	IState* currentState;
	// Initial state from which we suppose
	// all the other states are reachable
	IState* initialState;
	IController* owner;

	virtual void updateEventScheduling(IState* currentState, IState* nextState) = 0;
	virtual void merge(IFSM* other) = 0;
	virtual std::set<IState*> getStates() = 0;

public:
	virtual IState* getCurrentState() = 0;
	virtual void setCurrentState(IState* currentState) = 0;
	virtual IController* getOwner() = 0;
	virtual IState* next(Packet* msg) = 0;
	virtual IState* next(cMessage* event) = 0;
	virtual std::map<std::string, IState*> getStatesMap() = 0;

	virtual ~IFSM() = default;
};

} // namespace inet

#endif /* CONTROLLER_FSM_IFSM_H_ */
