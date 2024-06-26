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

#ifndef CONTROLLER_FSM_STATE_CONCRETE_OPSTATE_H_
#define CONTROLLER_FSM_STATE_CONCRETE_OPSTATE_H_

#include "../IState.h"

class IOperation;
class IController;

namespace inet {

class IFSM;

class OpState : public IState {
public:
	virtual IState* next(IFSM* machine, Packet* msg) override;
	virtual IState* next(IFSM* machine, cMessage* event) override;
	virtual void merge(IState* other, std::map<std::string, IState*> states) override;
	virtual void setTransitions(std::vector<std::shared_ptr<ITransition>>& transitions) override;
	virtual std::vector<std::shared_ptr<ITransition>>& getTransitions() override;

	OpState() {};
	OpState(const char* name);
	OpState(std::vector<std::shared_ptr<ITransition>> transitions, const char* name);
	virtual ~OpState();
};

} // namespace inet;

#endif /* CONTROLLER_FSM_STATE_CONCRETE_OPSTATE_H_ */
