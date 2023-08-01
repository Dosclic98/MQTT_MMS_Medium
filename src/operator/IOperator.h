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

#ifndef OPERATOR_IOPERATOR_H_
#define OPERATOR_IOPERATOR_H_

#include <omnetpp.h>
#include "../operation/IOperation.h"
#include "../result/IResult.h"
#include "../message/mms/MmsMessage_m.h"

namespace inet {

class IOperator {

	private:
		simsignal_t cmdSubSig;
		simsignal_t resPubSig;
		simsignal_t msgPubSig;
		cListener* cmdListener;
	protected:
		virtual void execute(IOperation* op) = 0;
		virtual void propagate(IResult* res) = 0;
		virtual void propagate(MmsMessage* msg) = 0;
public:
	virtual ~IOperator() = default;
};

}

#endif /* OPERATOR_IOPERATOR_H_ */
