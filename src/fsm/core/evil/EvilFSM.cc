/*
 * EvilFSM.cpp
 *
 *  Created on: Jan 18, 2023
 *      Author: Davide Savarro
 */

#include "EvilFSM.h"

namespace inet {

EvilFSM::EvilFSM(ServerEvilComp* owner, bool startFull):
	FSM(startFull ? Full::getInstance() : Inactive::getInstance())
{
	// Initialize all the state graph
	if(startFull) Inactive::getInstance();

	this->owner = owner;

	// Initialize all the loop-arcs found starting from the Inactive state using a BFS
	this->initLoops();

	// When the FSM is created we must execute the
	// 'enter' routine for the initial state
	this->getCurrentState()->enter(this);
}

// This method adds all the arcs that generate a loop in the graph (self-loop or not found through a BFS starting from the Inactive state)
void EvilFSM::initLoops() {
	CommandOnly::getInstance()->setTransitions({ std::make_pair(0.2, ReadOnly::getInstance()), std::make_pair(0.8, Full::getInstance()) });
	Full::getInstance()->setTransitions({ std::make_pair(1, Full::getInstance()) });
}

void EvilFSM::next() {
	getCurrentState()->exit(this);
	setCurrentState(getCurrentState()->next(this));
	getCurrentState()->enter(this);
}

Inibs* EvilFSM::getInibValues() {
	return dynamic_cast<EvilState*>(getCurrentState())->getInibValues();
}

EvilFSM::~EvilFSM()
{
	// TODO Auto-generated destructor stub
}

};
