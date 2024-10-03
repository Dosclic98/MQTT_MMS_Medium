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

#include "PlaceholderHttpAtkFactory.h"
#include "../../../../controller/attacker/HttpAttackerController.h"
#include "../../../attacker/concrete/PlaceholderHttpAtk.h"

using namespace inet;

void PlaceholderHttpAtkFactory::build(omnetpp::cEvent* event) {
    HttpAttackerController* controller = check_and_cast<HttpAttackerController*>(this->controller);

    PlaceholderHttpAtk* atkOp = new PlaceholderHttpAtk();
    controller->propagate(atkOp);
}

PlaceholderHttpAtkFactory::PlaceholderHttpAtkFactory(HttpAttackerController* controller) {
    this->controller = controller;
}

PlaceholderHttpAtkFactory::~PlaceholderHttpAtkFactory() {
    // TODO Auto-generated destructor stub
}

