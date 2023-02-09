/*
 * Middle.h
 *
 *  Created on: Feb 9, 2023
 *      Author: dosclic98
 */

#ifndef FSM_STATES_EVIL_CONCRETE_MIDDLE_H_
#define FSM_STATES_EVIL_CONCRETE_MIDDLE_H_

#include "Full.h"
#include "../EvilState.h"

namespace inet {

class Middle: virtual public EvilState  {
public:
	void exit(FSM* machine);
	void action(FSM* machine);

	virtual ~Middle();

	static EvilState* getInstance() {
		static Middle singleton;
		return &(singleton);
	}

private:
	Middle():
		FSMState( { std::make_pair(1, Full::getInstance()) } ),
		EvilState(EvilStateName::MIDDLE, new Inibs(0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5), { std::make_pair(1, Full::getInstance()) })
	{ }
	Middle(const Middle& other);
	Middle& operator=(const Middle& other);
};

};

#endif /* FSM_STATES_EVIL_CONCRETE_MIDDLE_H_ */
