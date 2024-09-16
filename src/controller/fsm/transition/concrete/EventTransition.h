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

#ifndef CONTROLLER_FSM_TRANSITION_CONCRETE_EVENTTRANSITION_H_
#define CONTROLLER_FSM_TRANSITION_CONCRETE_EVENTTRANSITION_H_

#include "../ITransition.h"
#include "../../state/IState.h"

namespace inet {

class EventTransition : public ITransition {
protected:
	cMessage* event;
	EventMatchType matchType;
	simtime_t delay;
	cExpression* delayExpression;

public:
	virtual bool matchesTransition(Packet* packet) override;
	virtual IState* execute(Packet* packet) override;
	virtual bool matchesTransition(cMessage* event) override;
	virtual IState* execute(cMessage* event) override;
	virtual void scheduleSelf() override;
	virtual void descheduleSelf() override;
	virtual bool isScheduled() override;
	virtual bool equals(ITransition* other) override;
	virtual void setDormant(bool isDormant) override;
	EventTransition(IOperationFactory* operationFactory, IState* arrivalState, cMessage* event,
	        EventMatchType matchType, simtime_t delay, cExpression* delayExpression = nullptr,
	        INode* canaryNode = nullptr, bool isDormant = false);
	virtual ~EventTransition() override;
};

};

#endif /* CONTROLLER_FSM_TRANSITION_CONCRETE_EVENTTRANSITION_H_ */
