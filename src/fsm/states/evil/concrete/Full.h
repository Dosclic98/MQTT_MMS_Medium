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
	void enter(FSM* machine);
	void exit(FSM* machine);
	void action(FSM* machine);

	virtual ~Full();

	static EvilState* getInstance();

private:
	static Full singleton;
	Full():
		FSMState( { std::make_pair(1.0, this) } ),
		EvilState(new Inibs(1,1,1,1,1,1), { std::make_pair(1.0, this) })
	{ }
	Full(const Full& other);
	Full& operator=(const Full& other);
};

};

#endif /* FSM_STATES_EVIL_CONCRETE_FULL_H_ */
