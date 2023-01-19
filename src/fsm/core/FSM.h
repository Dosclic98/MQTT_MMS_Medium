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
	inline void setCurrentState(FSMState* currentState) { this->currentState = currentState; };
	virtual ~FSM() {};
protected:
	inline FSM(FSMState* currentState) { this->currentState = currentState; }
private:
	FSMState* currentState;
};

};

#endif /* FSM_CORE_FSM_H_ */
