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

#ifndef __TX_MEDIUM_EXP_ATTACKNODE_H_
#define __TX_MEDIUM_EXP_ATTACKNODE_H_

#include <omnetpp.h>
#include "../../controller/IController.h"
#include "../INode.h"

namespace inet {

#define KIND_COMPLETED 0
#define KIND_NOTIFY_COMPLETED 1

class ITransition;

enum NodeType {
	AND = 0,
	OR = 1,
	BEGIN = 2,
	END = 3,
	DEFENSE = 4,
	STEP = 5
};

// A kind of attack for each attack step type
enum AttackType {
	NOTSET = -1,
	ACCESS = 0,
	NETSNI = 1,
	ADVINTHEMID = 2,
	READOP = 3,
	WRITEOP = 4,
	DELETEOP = 5,
	SPOOFREPMES = 6,
	UNACOMMES = 7,
	UNSPOWSYS = 8
};

class AttackNode : public omnetpp::cSimpleModule, public INode{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void refreshDisplay() const override;
    virtual std::vector<AttackNode*> getParents();
    virtual void updateActivation();
    virtual void executeStep();

  public:
    inline static std::vector<std::string> displayStrings = {
    		"i=block/uparrow",
    		"i=block/downarrow",
			"i=misc/node2,blue",
    		"i=status/excl3",
			"i=block/circle",
    		"i=block/square"
    };
    virtual void setNodeType(NodeType nodetType);
    virtual NodeType getNodeType();
    virtual bool isCompleted();
    virtual void setState(bool state);
    virtual void setAttackType(AttackType attackType);
    virtual void setTargetControllers(std::vector<IController*> targetControllers);
    virtual void updateCanary(ITransition* trans);
    virtual void checkForCompletion();
    virtual void notifyCompletion();
    virtual void scheduleCompletionDelay();

  private:
    NodeType nodeType;
    AttackType attackType;
    // Node state (active or not)
    bool state;
    // Node completion state (completed or not)
    bool completedState;
    std::vector<IController*> targetControllers;
    // Map <K, V> of <ITransition, boolean> that records if a certain transition has been
    // traversed to implement a time-to-completion statistic for the attack step
    std::map<ITransition*, bool> completionCanary;
    virtual bool isActive();
};

} // namespace inet

#endif
