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

#ifndef CONTROLLER_FSM_STATE_ISTATE_H_
#define CONTROLLER_FSM_STATE_ISTATE_H_

#include <omnetpp.h>
#include <cstring>
#include "inet/common/packet/Packet.h"
#include "../../../result/IResult.h"
#include "../transition/ITransition.h"

namespace inet {

class IFSM;

class IState {
protected:
	const char* name;
	std::vector<std::shared_ptr<ITransition>> transitions;

public:
	virtual void setTransitions(std::vector<std::shared_ptr<ITransition>>& transitions) = 0;
	virtual std::vector<std::shared_ptr<ITransition>>& getTransitions() = 0;
	virtual IState* next(IFSM* machine, Packet* msg) = 0;
	virtual IState* next(IFSM* machine, cMessage* event) = 0;
	virtual void merge(IState* other) = 0;
	virtual const char* getName() {
		return this->name;
	};

	virtual ~IState() = default;
};

};

#endif /* CONTROLLER_FSM_STATE_ISTATE_H_ */
