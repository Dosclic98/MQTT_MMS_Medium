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

class ServerEvilComp;

class EvilFSM: public virtual FSM {
public:
	EvilFSM(ServerEvilComp* owner, bool startFull);

	void next() override;
	void update(const MmsMessage* msg, int checkEveryK);
	Inibs* getInibValues();
	int getNumMessages();
	~EvilFSM() override;

	ServerEvilComp* owner;
private:
	int numMessages;
	void initLoops();
};

};

#endif /* FSM_CORE_EVILFSM_H_ */
