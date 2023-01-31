/*
 * CommandOnly.h
 *
 *  Created on: Jan 30, 2023
 *      Author: dosclic98
 */

#ifndef FSM_STATES_EVIL_CONCRETE_COMMANDONLY_H_
#define FSM_STATES_EVIL_CONCRETE_COMMANDONLY_H_

#include "Full.h"
#include "../EvilState.h"

namespace inet {

class CommandOnly: virtual public EvilState {
public:
	void exit(FSM* machine);
	void action(FSM* machine);

	virtual ~CommandOnly();

	static EvilState* getInstance() {
		static CommandOnly singleton;
		return &(singleton);
	}
private:
	//static CommandOnly singleton;

	CommandOnly():
		FSMState( { std::make_pair(1, Full::getInstance()) } ),
		EvilState("COMMANDONLY", new Inibs(0,0,0,0,0.5,0.5,0,0,0.5,0.5), { std::make_pair(1, Full::getInstance()) })
	{ }
	CommandOnly(const CommandOnly& other);
	CommandOnly& operator=(const CommandOnly& other);
};

};

#endif /* FSM_STATES_EVIL_CONCRETE_COMMANDONLY_H_ */
