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

#ifndef UTILS_LOADER_AG_AGLOADER_H_
#define UTILS_LOADER_AG_AGLOADER_H_

#include <omnetpp.h>
#include "../../../graph/attack/NodeDefs.h"
#include <fstream>
#include "../../../lib/nlohmann-json/single_include/nlohmann/json.hpp"

namespace inet {

using json = nlohmann::json;

class AGLoader {
public:
    AGLoader(std::string& path, std::string& fileName);
    virtual std::vector<NodeContent> load();
    virtual ~AGLoader();
private:
    json agData;
    json conMapData;
};

}; // namespace inet

#endif /* UTILS_LOADER_AG_AGLOADER_H_ */
