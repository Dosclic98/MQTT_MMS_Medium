/*
 * FSMState.h
 *
 *  Created on: Jan 18, 2023
 *      Author: dosclic98
 */

#ifndef FSM_STATES_FSMSTATE_H_
#define FSM_STATES_FSMSTATE_H_

#include <vector>
#include <bits/stdc++.h>
#include <random>


namespace inet {

class FSM;

class FSMState
{
public:
	virtual void enter(FSM* machine) = 0;
	virtual FSMState* next(FSM* machine) = 0;
	virtual void exit(FSM* machine) = 0;
	virtual void action(FSM* machine) = 0;
	virtual ~FSMState() {};

	inline std::vector<std::pair<float, FSMState*>> getTransitions() { return transitions; }
	inline void setTransitions(std::vector<std::pair<float, FSMState*>> trans) {
		transitions = trans;
	}

protected:
	inline FSMState(std::vector<std::pair<float, FSMState*>> transitions) {
		this->transitions = transitions;
	}

private:
	std::vector<std::pair<float, FSMState*>> transitions;
};

};

#endif /* FSM_STATES_FSMSTATE_H_ */
