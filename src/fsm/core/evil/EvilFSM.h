/*
 * EvilFSM.h
 *
 *  Created on: Jan 18, 2023
 *      Author: Davide Savarro
 */

#ifndef FSM_CORE_EVILFSM_H_
#define FSM_CORE_EVILFSM_H_

#include "../FSM.h"
#include "../../../server/mms/multi/ServerEvilComp.h"

namespace inet {

class EvilFSM: public FSM {
public:
	EvilFSM();
	void next() override;
	~EvilFSM() override;

	ServerEvilComp* owner;
};

};

#endif /* FSM_CORE_EVILFSM_H_ */
