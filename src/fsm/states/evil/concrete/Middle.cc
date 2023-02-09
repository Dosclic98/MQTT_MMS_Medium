/*
 * Middle.cc
 *
 *  Created on: Feb 9, 2023
 *      Author: dosclic98
 */

#include "Middle.h"
#include "../../../../server/mms/multi/ServerEvilComp.h"

namespace inet {

void Middle::exit(FSM* machine) {
	// Fa qualcosa all'uscita dello stato
	EvilFSM* evilMachine = check_and_cast<EvilFSM*>(machine);
}

void Middle::action(FSM* machine) {
	// Esegue un'azione caratteristica dello stato (es. se arriva un messaggio dal client
	// incrementa un accumulatore e verifica se generare una risposta falsa)
	EvilFSM* evilMachine = check_and_cast<EvilFSM*>(machine);
}

Middle::~Middle()
{
	// TODO Auto-generated destructor stub
}

};
