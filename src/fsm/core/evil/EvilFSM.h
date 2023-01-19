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
	}
	void next() override;
	Inibs* getInibValues();
	~EvilFSM() override;

	ServerEvilComp* owner;
};

};

#endif /* FSM_CORE_EVILFSM_H_ */
