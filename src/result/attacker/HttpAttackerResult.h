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

#ifndef RESULT_ATTACKER_HTTPATTACKERRESULT_H_
#define RESULT_ATTACKER_HTTPATTACKERRESULT_H_

#include "../IResult.h"

namespace inet {

class HttpAttackerResult : public IResult {
public:
	HttpAttackerResult(int opId, ResultOutcome result);
	virtual ~HttpAttackerResult();

	virtual IController* getControllerOwner() override;
	virtual void setControllerOwner(IController* controllerOwner) override;
};

} // namespace inet

#endif /* RESULT_ATTACKER_HTTPATTACKERRESULT_H_ */
