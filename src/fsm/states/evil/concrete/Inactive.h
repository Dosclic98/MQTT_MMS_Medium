/*
 * Inactive.h
 *
 *  Created on: Jan 18, 2023
 *      Author: dosclic98
 */

#ifndef FSM_STATES_EVIL_CONCRETE_INACTIVE_H_
#define FSM_STATES_EVIL_CONCRETE_INACTIVE_H_

#include "../EvilState.h"

namespace inet {

class Inactive: virtual public EvilState {
public:
	void enter(FSM* machine);
	void exit(FSM* machine);
	virtual ~Inactive();

	static EvilState* getInstance();

private:
	static Inactive singleton;
	Inactive():
		FSMState( { std::make_pair(1.0, this) } ),
		EvilState(new Inibs(0,0,0,0,0,0), { std::make_pair(1.0, this) })
	{ }
	Inactive(const Inactive& other);
	Inactive& operator=(const Inactive& other);
};

};

#endif /* FSM_STATES_EVIL_CONCRETE_INACTIVE_H_ */
