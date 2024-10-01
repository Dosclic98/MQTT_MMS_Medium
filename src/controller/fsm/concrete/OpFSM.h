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

#ifndef CONTROLLER_FSM_CONCRETE_OPFSM_H_
#define CONTROLLER_FSM_CONCRETE_OPFSM_H_

#include "../IFSM.h"
#include "../state/IState.h"

namespace inet {

class OpFSM : public IFSM {
protected:
	virtual void updateEventSchedulingAfterExecution(IState* currentState, IState* nextState) override;
	virtual void updateEventSchedulingAfterMerge(IState* currentState) override;
	virtual std::set<IState*> getStates() override;

public:
	virtual IState* getCurrentState() override;
	virtual void setCurrentState(IState* currentState) override;
	virtual IController* getOwner() override;
	virtual IState* next(Packet* msg) override;
	virtual IState* next(cMessage* event) override;
	virtual std::map<std::string, IState*> getStatesMap() override;
	virtual void merge(IFSM* other) override;
    virtual void addDormancyUpdate(cMessage* event, std::shared_ptr<ITransition> transition, bool newDormancyState) override;
    virtual bool removeDormancyUpdate(cMessage* event, std::shared_ptr<ITransition> transition) override;
    virtual bool cleanDormancyUpdate(cMessage* event) override;
    virtual bool updateDormancy(cMessage* event) override;

	OpFSM(IController* owner, IState* currentState, bool scheduleOnBuild = true);
	virtual ~OpFSM() override;
};

} // namespace inet

#endif /* CONTROLLER_FSM_CONCRETE_OPFSM_H_ */
