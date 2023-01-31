/*
 * ReadOnly.h
 *
 *  Created on: Jan 30, 2023
 *      Author: dosclic98
 */

#ifndef FSM_STATES_EVIL_CONCRETE_READONLY_H_
#define FSM_STATES_EVIL_CONCRETE_READONLY_H_

#include "Full.h"
#include "../EvilState.h"
#include "CommandOnly.h"

namespace inet {

class ReadOnly : virtual public EvilState {
public:
	void exit(FSM* machine);
	void action(FSM* machine);

	virtual ~ReadOnly();

	static EvilState* getInstance() {
		static ReadOnly singleton;
		return &(singleton);
	}

private:
	//static ReadOnly singleton;

	ReadOnly():
		FSMState( { std::make_pair(0.2, CommandOnly::getInstance()), std::make_pair(0.8, Full::getInstance()) } ),
		EvilState("READONLY", new Inibs(0.5,0.5,0.5,0.5,0,0,0.5,0.5,0,0), { std::make_pair(0.2, CommandOnly::getInstance()), std::make_pair(0.8, Full::getInstance()) })
	{ }
	ReadOnly(const ReadOnly& other);
	ReadOnly& operator=(const ReadOnly& other);
};

};

#endif /* FSM_STATES_EVIL_CONCRETE_READONLY_H_ */
