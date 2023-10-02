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

#ifndef CONTROLLER_FSM_FACTORY_IFSMFACTORY_H_
#define CONTROLLER_FSM_FACTORY_IFSMFACTORY_H_

namespace inet {

class IController;
class IFSM;

class IFSMFactory {
protected:
	IController* controller;

public:
	virtual IFSM* build() = 0;
	virtual ~IFSMFactory() = default;
};

} // namespace inet

#endif /* CONTROLLER_FSM_FACTORY_IFSMFACTORY_H_ */
