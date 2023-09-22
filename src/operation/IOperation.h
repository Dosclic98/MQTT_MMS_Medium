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

#ifndef OPERATION_IOPERATION_H_
#define OPERATION_IOPERATION_H_

#include <omnetpp.h>


namespace inet {

class IOperator;

class IOperation: public omnetpp::cObject {

protected:
	static int idCounter;

	int id;
	IOperator* operatorOwner;

public:

	virtual void execute() = 0;
	virtual IOperator* getOperatorOwner() = 0;
	virtual void setOperatorOwner(IOperator* operatorOwner) = 0;

	virtual ~IOperation() = default;
};

}


#endif /* OPERATION_IOPERATION_H_ */
