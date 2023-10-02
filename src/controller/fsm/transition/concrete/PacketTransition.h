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

#ifndef CONTROLLER_FSM_TRANSITION_CONCRETE_PACKETTRANSITION_H_
#define CONTROLLER_FSM_TRANSITION_CONCRETE_PACKETTRANSITION_H_

#include "../ITransition.h"
#include "inet/common/packet/PacketFilter.h"
#include "../../state/IState.h"

namespace inet {

class PacketTransition : public ITransition {
protected:
	PacketFilter packetFilter;

public:
	virtual bool matchesTransition(Packet* packet) override;
	virtual IState* execute(Packet* packet) override;
	virtual bool matchesTransition(cEvent* event) override;
	virtual IState* execute(cEvent* event) override;

	PacketTransition(IOperationFactory* operationFactory, IState* arrivalState, const char* expression);
	virtual ~PacketTransition();
};

} // namespace inet

#endif /* CONTROLLER_FSM_TRANSITION_CONCRETE_PACKETTRANSITION_H_ */
