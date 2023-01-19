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
	virtual FSMState* next(FSM* machine) {
		// TODO Using this RNG doesn't guarantee the correctly spaced number generation
		std::default_random_engine generator;
		std::uniform_real_distribution<double> distribution(0.0,1.0);

		std::vector<std::pair<float, FSMState*>> trs = this->getTransitions();
		std::sort(trs.begin(), trs.end());
		for(int i = 0; i < trs.size(); i++) {
			std::pair<float, FSMState*>& tmp = trs[i];
			float p = distribution(generator);
			if(tmp.second) {
				if(p < tmp.first) return tmp.second;
			} else {
				throw std::invalid_argument("Exception: the next state wasn't valid");
			}

		}
		return this;
	}
	virtual void exit(FSM* machine) = 0;
	virtual ~FSMState() {};

	inline std::vector<std::pair<float, FSMState*>> getTransitions() { return transitions; }

protected:
	inline FSMState(std::vector<std::pair<float, FSMState*>> transitions) { this->transitions = transitions; }

private:
	std::vector<std::pair<float, FSMState*>> transitions;
};

};

#endif /* FSM_STATES_FSMSTATE_H_ */
