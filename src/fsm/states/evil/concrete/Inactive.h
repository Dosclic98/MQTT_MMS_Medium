/*
 * Inactive.h
 *
 *  Created on: Jan 18, 2023
 *      Author: dosclic98
 */

#ifndef FSM_STATES_EVIL_CONCRETE_INACTIVE_H_
#define FSM_STATES_EVIL_CONCRETE_INACTIVE_H_

#include "Full.h"
#include "../EvilState.h"
#include "ReadOnly.h"
#include "CommandOnly.h"

namespace inet {
// Inactive attack state
class Inactive: virtual public EvilState {

public:
	void exit(FSM* machine);
	void action(FSM* machine);

	virtual ~Inactive();

	static EvilState* getInstance();

private:
	static Inactive singleton;
	Inactive():
		FSMState( { std::make_pair(0.5, ReadOnly::getInstance()),  std::make_pair(0.5, CommandOnly::getInstance()) } ),
		EvilState("INACTIVE", new Inibs(0,0,0,0,0,0,0,0,0,0), { std::make_pair(0.5, ReadOnly::getInstance()),  std::make_pair(0.5, CommandOnly::getInstance()) })
	{ }
	Inactive(const Inactive& other);
	Inactive& operator=(const Inactive& other);
};

};

#endif /* FSM_STATES_EVIL_CONCRETE_INACTIVE_H_ */
