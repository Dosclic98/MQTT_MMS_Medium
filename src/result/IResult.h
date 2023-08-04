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

#ifndef RESULT_IRESULT_H_
#define RESULT_IRESULT_H_

#include <omnetpp.h>

namespace inet {

enum ResultOutcome {
	FAIL, SUCCESS
};

class IController;

class IResult: public omnetpp::cObject{
protected:
	int opId;
	IController* controllerOwner;
	ResultOutcome result;

public:
	virtual IController* getControllerOwner() = 0;
	virtual void setControllerOwner(IController* controllerOwner) = 0;
	virtual ResultOutcome getResult();
	virtual void setResult(ResultOutcome result);

	virtual ~IResult() = default;
};

} // namespace omnetpp
#endif /* RESULT_IRESULT_H_ */
