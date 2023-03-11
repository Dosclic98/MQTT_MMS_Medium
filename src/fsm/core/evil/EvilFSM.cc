/*
 * EvilFSM.cpp
 *
 *  Created on: Jan 18, 2023
 *      Author: Davide Savarro
 */

#include "EvilFSM.h"

namespace inet {

EvilFSM::EvilFSM(ClientEvilComp* owner, bool startFull):
	FSM(startFull ? Full::getInstance() : Inactive::getInstance())
{
	numMessages = 0;
	// Initialize all the state graph
	if(startFull) Inactive::getInstance();

	this->owner = owner;

	// Initialize all the loop-arcs found starting from the Inactive state using a BFS
	// but just if no other EvilFSM has already initialized the graph
	if(!EvilFSM::isGraphInit) {
		this->initLoops();
		EvilFSM::isGraphInit = true;
	}


	// When the FSM is created we must execute the
	// 'enter' routine for the initial state
	this->getCurrentState()->enter(this);
}

// This method adds all the arcs that generate a loop in the graph (self-loop or not found through a BFS starting from the Inactive state)
void EvilFSM::initLoops() {

}

void EvilFSM::next() {
	EvilState* currentState = dynamic_cast<EvilState*>(getCurrentState());
	EvilState* nextState = dynamic_cast<EvilState*>(currentState->next(this));
	if(nextState->getStateName() != currentState->getStateName()) {
		numMessages = 0;
		getCurrentState()->exit(this);
		setCurrentState(nextState);
		getCurrentState()->enter(this);
	}
}

void EvilFSM::update(const MmsMessage* msg, int checkEveryK) {
	numMessages++;
	EV << "Num: " + std::to_string(numMessages) + "\n";
	if(numMessages % checkEveryK == 0) { next(); }
}

Inibs* EvilFSM::getInibValues() {
	return dynamic_cast<EvilState*>(getCurrentState())->getInibValues();
}

int EvilFSM::getNumMessages() { return numMessages; }

EvilFSM::~EvilFSM()
{
	// TODO Auto-generated destructor stub
}

};
