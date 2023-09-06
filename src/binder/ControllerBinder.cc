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
	for(auto i = idToPathName.begin(); i != idToPathName.end(); i++) {
		if(strcmp(i->second, pathName) == 0) {
			return idToRef[i->first];
		}
	}
	return nullptr;
}

const char* ControllerBinder::getPathName(int id) {
	return idToPathName[id];
}

void ControllerBinder::addRef(int id, IController* ref) {
	idToRef[id] = ref;
}

void ControllerBinder::removeRef(int id) {
	idToRef.erase(id);
}

void ControllerBinder::addPathName(int id, const char* pathName) {
	idToPathName[id] = pathName;
}

void ControllerBinder::removePathName(int id) {
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

