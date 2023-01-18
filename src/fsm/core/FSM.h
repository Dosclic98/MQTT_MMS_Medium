/*
 * FSM.h
 *
 *  Created on: Jan 18, 2023
 *      Author: dosclic98
 */

#ifndef FSM_CORE_FSM_H_
#define FSM_CORE_FSM_H_

#include "../states/FSMState.h"

namespace inet {

class FSM {
public:
	virtual void next() = 0;
	inline FSMState* getCurrentState() { return currentState; };
	virtual ~FSM() {};
private:
	FSMState* currentState;
};

};

#endif /* FSM_CORE_FSM_H_ */
