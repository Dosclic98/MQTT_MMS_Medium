/*
 * FSMState.h
 *
 *  Created on: Jan 18, 2023
 *      Author: dosclic98
 */

#ifndef FSM_STATES_FSMSTATE_H_
#define FSM_STATES_FSMSTATE_H_

#include "../core/FSM.h"
#include <vector>

namespace inet {

class FSM;

class FSMState
{
public:
	virtual void enter(FSM* machine) {};
	virtual FSMState* next(FSM* machine) {return this;};
	virtual void exit(FSM* machine) {};
	virtual ~FSMState() {};
protected:
	std::vector<std::pair<float, FSMState*>> transitions;
};

};

#endif /* FSM_STATES_FSMSTATE_H_ */
