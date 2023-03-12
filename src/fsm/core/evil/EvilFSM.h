/*
 * EvilFSM.h
 *
 *  Created on: Jan 18, 2023
 *      Author: Davide Savarro
 */

#ifndef FSM_CORE_EVILFSM_H_
#define FSM_CORE_EVILFSM_H_

#include "../FSM.h"
#include "../../../message/mms/MmsMessage_m.h"
#include "../../states/evil/concrete/Inactive.h"


namespace inet {

class ClientEvilComp;

class EvilFSM: public virtual FSM {
public:
	EvilFSM(ClientEvilComp* owner, bool startFull);

	void next() override;
	void update(const MmsMessage* msg, int checkEveryK);
	Inibs* getInibValues();
	int getNumMessages();
	~EvilFSM() override;

	ClientEvilComp* owner;
	static bool isGraphInit;
private:
	int numMessages;
	void initLoops();
};

};

#endif /* FSM_CORE_EVILFSM_H_ */
