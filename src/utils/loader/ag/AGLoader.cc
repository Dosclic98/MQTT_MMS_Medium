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

#include "AGLoader.h"
#include <fstream>
#include "../../../lib/nlohmann-json/single_include/nlohmann/json.hpp"

using namespace inet;
using json = nlohmann::json;

Define_Module(AGLoader);

void AGLoader::initialize() {
    // Initialize logger after the AG has been intialized
    std::string path = par("path").stdstringValue();
    std::string fileName = par("fileName").stdstringValue();

    std::ifstream f(path + fileName);
    json data = json::parse(f);
}

void AGLoader::handleMessage(cMessage *msg) {
    // TODO - Generated method body
}
