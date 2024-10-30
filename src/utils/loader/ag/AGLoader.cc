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

using namespace inet;
using namespace omnetpp;

namespace inet {

void from_json(const json& j, NodeContent& nc) {
    try {
        j.at("name").get_to(nc.displayName);
    } catch(json::exception e) {
        cException exc = cException(e.what());
        exc.prependMessage("displayName initialization error");
        throw exc;
    }
    try {
        j.at("nodeType").get_to(nc.nodeType);
    } catch(json::exception e) {
        cException exc = cException(e.what());
        exc.prependMessage("nodeType initialization error");
        throw exc;
    } try {
        j.at("attackType").get_to(nc.attackType);
    } catch(json::exception e) {
        cException exc = cException(e.what());
        exc.prependMessage("attackType initialization error");
        throw exc;
    } try {
        j.at("state").get_to(nc.state);
    } catch(json::exception e) {
        cException exc = cException(e.what());
        exc.prependMessage("sate initialization error");
        throw exc;
    } try {
        j.at("delayExpr").get_to(nc.completionDelayExpr);
    } catch(json::exception e) {
        cException exc = cException(e.what());
        exc.prependMessage("completionDelayExpr initialization error");
        throw exc;
    } try {
        j.at("childNodes").get_to(nc.children);
    } catch(json::exception e) {
        cException exc = cException(e.what());
        exc.prependMessage("childNodes initialization error");
        throw exc;
    }

}

};

AGLoader::AGLoader(std::string& path, std::string& fileName) {
    std::ifstream fAg(path + fileName);
    std::ifstream fMapCont("controllerMappings.json");
    agData = json::parse(fAg);
    conMapData = json::parse(fMapCont);
}

std::vector<NodeContent> AGLoader::load() {
    auto ag = agData.at("attackGraph");
    auto conts = conMapData.at("attackGraph");

    std::vector<NodeContent> attackGraph;
    for (auto n : ag) {
        NodeContent node = n.template get<NodeContent>();
        for(auto cont : conts) {
            if(cont.at("name") == node.displayName) {
                node.targetControllerList = cont.at("targetControllers");
            }
        }
        attackGraph.push_back(node);
    }

    return attackGraph;
}

AGLoader::~AGLoader() {
    // TODO Auto-generated destructor stub
}

