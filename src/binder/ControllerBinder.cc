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

#include "ControllerBinder.h"
#include "../controller/IController.h"

using namespace inet;

Define_Module(ControllerBinder);

void ControllerBinder::initialize(int stage) {
    if (stage == inet::INITSTAGE_LOCAL) {
        networkName = std::string(getSystemModule()->getName());
    }
}

void ControllerBinder::registerController(IController* controller) {
	cModule* module = check_and_cast<cModule*>(controller);
	addRef(module->getId(), controller);
	std::string tmpName = module->getFullPath();
	const char* pathName = strcpy(new char[tmpName.length() + 1], tmpName.c_str());
	addPathName(module->getId(), pathName);
}

void ControllerBinder::unregisterController(int id) {
	removeRef(id);
	removePathName(id);
}

IController* ControllerBinder::getRefByPathName(const char* pathName) {
	std::string pathNameStr = std::string(pathName);
	if(pathNameStr.rfind(this->getSimulation()->getSystemModule()->getName(), 0) != 0) {
		pathNameStr = std::string(this->getSimulation()->getSystemModule()->getName()) + "." + pathNameStr;
	}
	for(auto i = idToPathName.begin(); i != idToPathName.end(); i++) {
		if(strcmp(i->second, pathNameStr.c_str()) == 0) {
			return idToRef[i->first];
		}
	}
	return nullptr;
}

const char* ControllerBinder::getPathName(int id) {
	if(idToPathName.find(id) == idToPathName.end()) {
		throw cRuntimeError("PathName with id %d not found\n", id);
	}
	return idToPathName[id];
}

void ControllerBinder::addRef(int id, IController* ref) {
	if(idToRef.find(id) != idToRef.end()) {
		throw cRuntimeError("Controller reference with id %d already present\n", id);
	}
	idToRef[id] = ref;
}

void ControllerBinder::removeRef(int id) {
	if(idToRef.find(id) == idToRef.end()) {
		throw cRuntimeError("Controller reference with id %d not present: impossible to remove\n", id);
	}
	idToRef.erase(id);
}

void ControllerBinder::addPathName(int id, const char* pathName) {
	if(idToPathName.find(id) != idToPathName.end()) {
		throw cRuntimeError("PathName with id %d already present: cannot add\n", id);
	}
	EV << "Adding module: " << pathName << "\n";
	idToPathName[id] = pathName;
}

void ControllerBinder::removePathName(int id) {
	if(idToPathName.find(id) == idToPathName.end()) {
		throw cRuntimeError("PathName with id %d not present: impossible to remove\n", id);
	}
	delete [] idToPathName[id];
	idToPathName.erase(id);
}

void ControllerBinder::finish() {
	for(auto i = idToPathName.begin(); i != idToPathName.end(); i++) {
		delete [] i->second;
	}
	idToPathName.clear();
	idToRef.clear();
}

