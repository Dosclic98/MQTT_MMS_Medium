/*
 * EvilFSM.cpp
 *
 *  Created on: Jan 18, 2023
 *      Author: Davide Savarro
 */

#include "EvilFSM.h"

namespace inet {

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
