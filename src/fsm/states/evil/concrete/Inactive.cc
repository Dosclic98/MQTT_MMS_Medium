/*
 * Inactive.cc
 *
 *  Created on: Jan 18, 2023
 *      Author: dosclic98
 */

#include "Inactive.h"

namespace inet {

Inactive::Inactive()
{
	// TODO Auto-generated constructor stub

}

void Inactive::enter(EvilFSM* machine) {
	// Schedula l'evento di cambio stato
}

void Inactive::exit(EvilFSM* machine) {
	// Fa qualcosa all'uscita dello stato
}

EvilState& Inactive::getInstance() {
	static Inactive singleton;
	return singleton;
}

Inactive::~Inactive()
{
	// TODO Auto-generated destructor stub
}

};

