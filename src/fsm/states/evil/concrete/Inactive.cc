/*
 * Inactive.cc
 *
 *  Created on: Jan 18, 2023
 *      Author: dosclic98
 */

#include "Inactive.h"
#include "../../../../server/mms/multi/ServerEvilComp.h"

namespace inet {

void Inactive::exit(FSM* machine) {
	// Fa qualcosa all'uscita dello stato
	EvilFSM* evilMachine = check_and_cast<EvilFSM*>(machine);
}

void Inactive::action(FSM* machine) {
	// Esegue un'azione caratteristica dello stato (es. se arriva un messaggio dal client
	// incrementa un accumulatore e verifica se generare una risposta falsa)
	EvilFSM* evilMachine = check_and_cast<EvilFSM*>(machine);
}

Inactive::~Inactive() {

}

};

