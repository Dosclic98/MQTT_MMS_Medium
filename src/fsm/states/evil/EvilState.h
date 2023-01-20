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

class EvilFSM;

class EvilState: public virtual FSMState {
public:
	virtual ~EvilState() {};
	virtual FSMState* next(FSM* machine) override;
	void enter(FSM* machine) override;

	inline Inibs* getInibValues() { return inibValues.get(); }

protected:
	inline EvilState(Inibs* inibValues, std::vector<std::pair<float, FSMState*>> transitions):
		FSMState( transitions )
	{ this->inibValues = std::unique_ptr<Inibs>(inibValues); }

protected:
	std::unique_ptr<Inibs> inibValues;

};

};

#endif /* EVIL_STATES_FSMSTATE_H_ */
