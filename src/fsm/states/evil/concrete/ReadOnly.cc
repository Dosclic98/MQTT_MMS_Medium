/*
 * ReadOnly.cc
 *
 *  Created on: Jan 30, 2023
 *      Author: dosclic98
 */

#include "ReadOnly.h"
#include "../../../../server/mms/multi/ServerEvilComp.h"

namespace inet {

void ReadOnly::exit(FSM* machine) {
	// Fa qualcosa all'uscita dello stato
	EvilFSM* evilMachine = check_and_cast<EvilFSM*>(machine);
}

void ReadOnly::action(FSM* machine) {
	// Esegue un'azione caratteristica dello stato (es. se arriva un messaggio dal client
	// incrementa un accumulatore e verifica se generare una risposta falsa)
	EvilFSM* evilMachine = check_and_cast<EvilFSM*>(machine);
}

ReadOnly::~ReadOnly()
{ }

};

