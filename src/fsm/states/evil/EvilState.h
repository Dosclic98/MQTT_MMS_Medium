/*
 * FSMState.h
 *
 *  Created on: Jan 18, 2023
 *      Author: dosclic98
 */

#ifndef EVIL_STATES_FSMSTATE_H_
#define EVIL_STATES_FSMSTATE_H_

#include "../FSMState.h"
#include "../../utils/Inibs.h"

namespace inet {

enum EvilStateName {
	INACTIVE = 0,
	MIDDLE = 1,
	READONLY = 2,
	COMMANDONLY = 3,
	FULL = 4
};

class EvilFSM;

class EvilState: public virtual FSMState {
public:
	virtual ~EvilState() {};
	virtual FSMState* next(FSM* machine) override;
	void enter(FSM* machine) override;
	std::vector<float> calcProb(int numMessages, std::vector<std::pair<float, FSMState*>> transitions);

	inline Inibs* getInibValues() { return inibValues.get(); }
	inline EvilStateName getStateName() { return stateName; }

	static const char* stateNames[];

protected:
	inline EvilState(EvilStateName stateName, Inibs* inibValues, std::vector<std::pair<float, FSMState*>> transitions):
		FSMState( transitions ) {
		this->stateName = stateName;
		this->inibValues = std::unique_ptr<Inibs>(inibValues);
		q = 6;
		k = 4;
	}

protected:
	std::unique_ptr<Inibs> inibValues;
	EvilStateName stateName;
	// Parameters of the sigmoid function
	float q;
	float k;
};

};

#endif /* EVIL_STATES_FSMSTATE_H_ */
