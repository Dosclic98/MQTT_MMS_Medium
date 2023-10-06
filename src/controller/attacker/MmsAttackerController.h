//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __TX_MEDIUM_EXP_MMSATTACKERCONTROLLER_H_
#define __TX_MEDIUM_EXP_MMSATTACKERCONTROLLER_H_

#include <omnetpp.h>
#include "../IController.h"
#include "inet/common/packet/ChunkQueue.h"
#include "../../utils/factories/mms/MmsMessageCopier.h"
#include "../../utils/logger/evil/EvilLogger.h"

namespace inet {

#define MSGKIND_CONNECT    0
#define MSGKIND_SEND       1

class MmsAttackerController : public cSimpleModule, public IController {
  public:
	MmsMessageCopier* messageCopier;
	cMessage* sendMsgEvent;

	bool isSocketConnected;

	cMessage* timeoutMsg;

	// Signals management
	simsignal_t genericFakeReqResSignal;
	simsignal_t measureBlockSignal;
	simsignal_t measureCompromisedSignal;
	simsignal_t readResponseBlockSignal;
	simsignal_t readResponseCompromisedSignal;
	simsignal_t commandResponseBlockSignal;
	simsignal_t commandResponseCompromisedSignal;
	simsignal_t readRequestBlockSignal;
	simsignal_t readRequestCompromisedSignal;
	simsignal_t commandRequestBlockSignal;
	simsignal_t commandRequestCompromisedSignal;

	double readResponseBlockProb;
	double readResponseCompromisedProb;
	double commandResponseBlockProb;
	double commandResponseCompromisedProb;

	double readRequestBlockProb;
	double readRequestCompromisedProb;
	double commandRequestBlockProb;
	double commandRequestCompromisedProb;

	double measureBlockProb;
	double measureCompromisedProb;

	virtual ~MmsAttackerController();

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    virtual void next(Packet* msg = nullptr) override;
    virtual void propagate(IOperation* op) override;
    virtual void evalRes(IResult* res) override;

    void enqueueNSchedule(IOperation* operation) override;
    bool isAtkLogging();
    void log(const MmsMessage* msg, const char* evilStateName, simtime_t timestamp);

    int getNumGenReq();
    int getFakeGenReqThresh();
    void setNumGenReq(int numGenReq);

  protected:
	ChunkQueue queue;

	// For logging purposes
	bool isLogging;
	EvilLogger* logger;

	int fakeGenReqThresh;
	int numGenReq;
};

} // namespace inet

#endif
