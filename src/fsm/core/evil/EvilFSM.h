/*
 * EvilFSM.h
 *
 *  Created on: Jan 18, 2023
 *      Author: Davide Savarro
 */

#ifndef FSM_CORE_EVILFSM_H_
#define FSM_CORE_EVILFSM_H_

#include "../FSM.h"
#include "../../states/evil/concrete/Inactive.h"


namespace inet {

class ServerEvilComp;

class EvilFSM: public virtual FSM {
public:
	EvilFSM(ServerEvilComp* owner):
		FSM(Inactive::getInstance())
	{
		this->owner = owner;
		// When the FSM is created we must execute the
		// 'enter' routine for the initial state
		this->getCurrentState()->enter(this);
	}
	void next() override;
	Inibs* getInibValues();
	~EvilFSM() override;

	ServerEvilComp* owner;
};

};

#endif /* FSM_CORE_EVILFSM_H_ */