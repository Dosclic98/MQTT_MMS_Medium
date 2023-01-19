/*
 * Inactive.cc
 *
 *  Created on: Jan 18, 2023
 *      Author: dosclic98
 */

#include "Inactive.h"
#include "../../../../server/mms/multi/ServerEvilComp.h"

namespace inet {

Inactive Inactive::singleton = Inactive();

EvilState* Inactive::getInstance() {
	return &(singleton);
}

void Inactive::enter(FSM* machine) {
	// Schedula l'evento di cambio stato
	EvilFSM* evilMachine = check_and_cast<EvilFSM*>(machine);
}

void Inactive::exit(FSM* machine) {
	// Fa qualcosa all'uscita dello stato
	EvilFSM* evilMachine = check_and_cast<EvilFSM*>(machine);
}

Inactive::~Inactive() {

}

};

