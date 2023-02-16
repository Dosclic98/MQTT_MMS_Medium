/*
 * EvilState.cc
 *
 *  Created on: Jan 20, 2023
 *      Author: dosclic98
 */

#include "EvilState.h"
#include "../../../server/mms/multi/ServerEvilComp.h"

namespace inet {

const char* EvilState::stateNames[] = {"INACTIVE", "MIDDLE", "READONLY", "COMMANDONLY", "FULL"};

// This method is implemented here instead of in the FSMState because it needs a reference to the owner of
// the FSM which is known at this level of the hierarchy
FSMState* EvilState::next(FSM* machine) {
	EvilFSM* evilMachine = check_and_cast<EvilFSM*>(machine);
	ServerEvilComp* owner = evilMachine->owner;

	std::vector<std::pair<float, FSMState*>> trns = this->getTransitions();
	std::sort(trns.begin(), trns.end());
	// Calculate transition probabilities
	std::vector<float> probs = calcProb(evilMachine->getNumMessages(), trns);
	float base = 0;
	float p = owner->uniform(0.0, 1.0);
	for(int i = 0; i < trns.size(); i++) {
		EV << "Prob: " + std::to_string(probs[i]) + "\n";
		std::pair<float, FSMState*>& tmp = trns[i];
		if(tmp.second) {
			if(p >= base && p < base + probs[i]) {
				EV << "Change prob: " + std::to_string(base + probs[i]) + "\n";
				EV << "Change p: " + std::to_string(p) + "\n";
				return tmp.second;
			}
			base += p;
		} else {
			throw std::invalid_argument("Exception: the next state wasn't valid");
		}

	}
	return this;
}

std::vector<float> EvilState::calcProb(int numMessages, std::vector<std::pair<float, FSMState*>> trns) {
	float tot = 0;
	for(int i = 0; i < trns.size(); i++) { tot += trns[i].first; }

	float redProb = 1 / (1 + exp(((-1/q)*numMessages)+k));
	std::vector<float> probs;
	for(int i = 0; i < trns.size(); i++) {
		probs.push_back((redProb * trns[i].first) / tot);
	}
	return probs;
}

// Default behaviour when entering the new state (schedule the next state change)
void EvilState::enter(FSM* machine) {
	EvilFSM* evilMachine = check_and_cast<EvilFSM*>(machine);
	ServerEvilComp* serverComp = evilMachine->owner;
}

};

