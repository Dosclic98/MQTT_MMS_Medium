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

#ifndef CONTROLLER_CLIENT_MMSCLIENTCONTROLLER_H_
#define CONTROLLER_CLIENT_MMSCLIENTCONTROLLER_H_

#include "../IController.h"

namespace inet  {

class MmsClientController : public cSimpleModule, public IController {
private:
	int idCounter = 0;
public:
	MmsClientController();
	virtual ~MmsClientController();

	void initialize() override;
	void handleMessage(cMessage* msg) override;

	virtual void next(MmsMessage* msg = nullptr) override;
	virtual void propagate(IOperation* op) override;
	virtual void evalRes(IResult* res) override;
};

}

#endif /* CONTROLLER_CLIENT_MMSCLIENTCONTROLLER_H_ */
