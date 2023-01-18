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

class Inactive: public EvilState {
public:
	void enter(EvilFSM* machine);
	void exit(EvilFSM* machine);
	virtual ~Inactive();

	static EvilState& getInstance();

private:
	Inactive();
	Inactive(const Inactive& other);
	Inactive& operator=(const Inactive& other);
};

};

#endif /* FSM_STATES_EVIL_CONCRETE_INACTIVE_H_ */
