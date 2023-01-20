/*
 * Full.cc
 *
 *  Created on: Jan 20, 2023
 *      Author: dosclic98
 */

#include "Full.h"
#include "../../../../server/mms/multi/ServerEvilComp.h"

namespace inet {

Full Full::singleton = Full();

EvilState* Full::getInstance() {
	return &(singleton);
}

void Full::exit(FSM* machine) {
	// Fa qualcosa all'uscita dello stato
	EvilFSM* evilMachine = check_and_cast<EvilFSM*>(machine);
}

void Full::action(FSM* machine) {
	// Esegue un'azione caratteristica dello stato (es. se arriva un messaggio dal client
	// incrementa un accumulatore e verifica se generare una risposta falsa)
	EvilFSM* evilMachine = check_and_cast<EvilFSM*>(machine);
}

Full::~Full()
{
	// TODO Auto-generated destructor stub
}

};
