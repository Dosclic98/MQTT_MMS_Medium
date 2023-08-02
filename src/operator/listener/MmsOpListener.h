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

#ifndef OPERATOR_LISTENER_MMSOPLISTENER_H_
#define OPERATOR_LISTENER_MMSOPLISTENER_H_

#include <omnetpp.h>
#include "../IOperator.h"

namespace inet {

class MmsOpListener : public omnetpp::cListener{
public:
	IOperator* parent;

	MmsOpListener(IOperator* parent);
	virtual ~MmsOpListener();

	virtual void receiveSignal(omnetpp::cComponent* source, omnetpp::simsignal_t signalID, omnetpp::cObject* value, omnetpp::cObject *obj) override;
};

} // namespace inet

#endif /* OPERATOR_LISTENER_MMSOPLISTENER_H_ */
