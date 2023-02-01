/*
 * Full.h
 *
 *  Created on: Jan 20, 2023
 *      Author: dosclic98
 */

#ifndef FSM_STATES_EVIL_CONCRETE_FULL_H_
#define FSM_STATES_EVIL_CONCRETE_FULL_H_

#include "../EvilState.h"

namespace inet {
// Full attack power state
class Full: virtual public EvilState {

public:
	void exit(FSM* machine);
	void action(FSM* machine);

	virtual ~Full();

	static EvilState* getInstance() {
		static Full singleton;
		return &(singleton);
	}

private:
	Full():
		FSMState( {  } ),
		EvilState(EvilStateName::FULL, new Inibs(1,1,1,1,1,1,1,1,1,1), {  })
	{ }
	Full(const Full& other);
	Full& operator=(const Full& other);
};

};

#endif /* FSM_STATES_EVIL_CONCRETE_FULL_H_ */
