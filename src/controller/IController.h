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

#ifndef __TX_MEDIUM_EXP_ICONTROLLER_H_
#define __TX_MEDIUM_EXP_ICONTROLLER_H_

#include <omnetpp.h>
#include "../message/mms/MmsMessage_m.h"
#include "../operator/IOperator.h"
#include "../result/IResult.h"

namespace inet {

/**
 * TODO - Generated class
 */
class IController {
  protected:
	simsignal_t cmdPubSig;
	cListener* resListener;
	cListener* msgListener;
	// TODO Add the Graph and the FSM

  public:
    virtual void next(MmsMessage* msg = nullptr) = 0;
    virtual void propagate(IOperation* operation) = 0;
    virtual void evalRes(IResult* res) = 0;
};

} // namespace inet

#endif
