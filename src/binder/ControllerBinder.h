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

#ifndef __TX_MEDIUM_EXP_CONTROLLERBINDER_H_
#define __TX_MEDIUM_EXP_CONTROLLERBINDER_H_

#include <omnetpp.h>
#include <inet/networklayer/contract/ipv4/Ipv4Address.h>

namespace inet {

class IController;

class ControllerBinder : public omnetpp::cSimpleModule {
private:
	std::string networkName;
	std::map<int, IController*> idToRef;
	std::map<int, const char*> idToPathName;

	virtual void addRef(int id, IController* ref);
	virtual void removeRef(int id);
	virtual void addPathName(int id, const char* pathName);
	virtual void removePathName(int id);



protected:
	virtual void initialize(int stage) override;
	virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
	virtual void handleMessage(omnetpp::cMessage *msg) override { }
	virtual void finish() override;

public:
	virtual void registerController(IController* controller);
	virtual void unregisterController(int id);
	virtual IController* getRefByPathName(const char* pathName);
	virtual const char* getPathName(int id);

};

} // namespace inet

#endif
