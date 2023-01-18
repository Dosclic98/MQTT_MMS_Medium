/*
 * FSMState.h
 *
 *  Created on: Jan 18, 2023
 *      Author: dosclic98
 */

#ifndef EVIL_STATES_FSMSTATE_H_
#define EVIL_STATES_FSMSTATE_H_

#include "../../core/evil/EvilFSM.h"
#include <bits/stdc++.h>

namespace inet {

// Struct contenente i valori d'inibizione
typedef struct{
	float measureBlockInib = 0;
	float measureCompromisedInib = 0;
	float readResponseBlockInib = 0;
	float readResponseCompromisedInib = 0;
	float commandResponseBlockInib = 0;
	float commandResponseCompromisedInib = 0;
}Inibs;

class EvilState: public FSMState
{
public:
	virtual void enter(EvilFSM* machine) = 0;

	inline EvilState* next(EvilFSM* machine) {
		std::sort(transitions.begin(), transitions.end());
		for(int i = 0; i < transitions.size(); i++) {
			std::pair<float, FSMState*>* tmp = &transitions[i];
			float p = machine->owner->uniform(0, 1);
			if(p < tmp->first) return check_and_cast<EvilState*>(tmp->second);
		}
		return this;
	}

	virtual void exit(EvilFSM* machine) = 0;
	virtual ~EvilState() {};

	inline Inibs* getInibValues() { return &inibValues; }

private:
	Inibs inibValues;
};

};

#endif /* EVIL_STATES_FSMSTATE_H_ */
