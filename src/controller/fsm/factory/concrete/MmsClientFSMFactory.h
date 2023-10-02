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

#ifndef CONTROLLER_FSM_FACTORY_CONCRETE_MMSCLIENTFSMFACTORY_H_
#define CONTROLLER_FSM_FACTORY_CONCRETE_MMSCLIENTFSMFACTORY_H_

#include "../IFSMFactory.h"

namespace inet {

class MmsClientController;

class MmsClientFSMFactory : public IFSMFactory {
public:
	virtual IFSM* build() override;
	virtual ~MmsClientFSMFactory();
	MmsClientFSMFactory(MmsClientController* controller);
};

} // namespace inet

#endif /* CONTROLLER_FSM_FACTORY_CONCRETE_MMSCLIENTFSMFACTORY_H_ */
