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

#ifndef OPERATION_FACTORY_PACKET_CONCRETE_FORWARDMMSMESSAGETOSERVERFACTORY_H_
#define OPERATION_FACTORY_PACKET_CONCRETE_FORWARDMMSMESSAGETOSERVERFACTORY_H_

#include "../PacketOperationFactory.h"

namespace inet {

class ForwardMmsMessageToServerFactory : public PacketOperationFactory {
public:
	virtual IOperation* build(Packet* packet) override;

	ForwardMmsMessageToServerFactory();
	virtual ~ForwardMmsMessageToServerFactory();
};

} // namespace inet

#endif /* OPERATION_FACTORY_PACKET_CONCRETE_FORWARDMMSMESSAGETOSERVERFACTORY_H_ */
