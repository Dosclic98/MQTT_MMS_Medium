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

#ifndef __TX_MEDIUM_EXP_MMSSERVERCONTROLLER_H_
#define __TX_MEDIUM_EXP_MMSSERVERCONTROLLER_H_

#include "../IController.h"
#include "inet/common/packet/ChunkQueue.h"


namespace inet {

class MmsServerController : public cSimpleModule, public IController {
  public:
	virtual void enqueueNSchedule(IOperation* operation) override;
	virtual void scheduleEvent(cMessage* event, simtime_t delay) override;
	virtual void descheduleEvent(cMessage* event) override;
	virtual void insertMmsSubscriber(int connId, int evilServerConnId);
	virtual void removeMmsSubscriber(int connId);
	virtual std::list<std::pair<int,int>>& getMmsSubscriberList();
	virtual cMessage* getSendMeasuresEvent();

	MmsServerController();
	virtual ~MmsServerController();
  protected:
	// Gestione invio misure
	cMessage* sendDataEvent;
	cMessage* departureEvent;

	// Gestione della sottoscrizione del client per ricezione misure
	std::list< std::pair<int,int> > clientConnIdList;

	ChunkQueue queue;

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    virtual void next(Packet* msg = nullptr) override;
    virtual void propagate(IOperation* op) override;
    virtual void evalRes(IResult* res) override;
};

} // namespace inet

#endif
