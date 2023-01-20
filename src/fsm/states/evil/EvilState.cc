/*
 * EvilState.cc
 *
 *  Created on: Jan 20, 2023
 *      Author: dosclic98
 */

#include "EvilState.h"
#include "../../../server/mms/multi/ServerEvilComp.h"

namespace inet {
// This method is implemented here instead of in the FSMState because it needs a reference to the owner of
// the FSM which is known at this level of the hierarchy
FSMState* EvilState::next(FSM* machine) {
	EvilFSM* evilMachine = check_and_cast<EvilFSM*>(machine);
	ServerEvilComp* owner = evilMachine->owner;

	std::vector<std::pair<float, FSMState*>> trs = this->getTransitions();
	std::sort(trs.begin(), trs.end());
	for(int i = 0; i < trs.size(); i++) {
		std::pair<float, FSMState*>& tmp = trs[i];
		float p = owner->uniform(0.0, 1.0);
		if(tmp.second) {
			if(p < tmp.first) return tmp.second;
		} else {
			throw std::invalid_argument("Exception: the next state wasn't valid");
		}

	}
	return this;
}

};

