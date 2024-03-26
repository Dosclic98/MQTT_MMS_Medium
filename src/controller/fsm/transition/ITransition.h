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

#ifndef CONTROLLER_FSM_TRANSITION_ITRANSITION_H_
#define CONTROLLER_FSM_TRANSITION_ITRANSITION_H_

#include "../../../operation/factory/IOperationFactory.h"
#include "../../../controller/IController.h"

namespace inet {

enum EventMatchType {
	Kind = 0,
	Ref = 1
};

class IState;
class INode;

class ITransition {

protected:
	IOperationFactory* operationFactory;
	IState* arrivalState;
	// The eventual attack node on which the canary completion is performed
	INode* canaryNode = nullptr;

public:
	virtual bool matchesTransition(Packet* packet) = 0;
	virtual IState* execute(Packet* packet) = 0;
	virtual bool matchesTransition(cMessage* event) = 0;
	virtual IState* execute(cMessage* event) = 0;
	virtual void scheduleSelf() = 0;
	virtual void descheduleSelf() = 0;
	virtual bool isScheduled() = 0;
	virtual bool equals(ITransition* other) = 0;
	virtual void setArrivalState(IState* arrivalState) {
		this->arrivalState = arrivalState;
	};
	virtual IState* getArrivalState() {
		return arrivalState;
	};

	virtual ~ITransition() = default;
};

} // namespace inet

#endif /* CONTROLLER_FSM_TRANSITION_ITRANSITION_H_ */
