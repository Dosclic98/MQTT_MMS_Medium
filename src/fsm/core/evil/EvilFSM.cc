/*
 * EvilFSM.cpp
 *
 *  Created on: Jan 18, 2023
 *      Author: Davide Savarro
 */

#include "EvilFSM.h"

namespace inet {

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
