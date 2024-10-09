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

#include "HttpClientEvilOperator.h"
#include "../listener/FromSerOpListener.h"

using namespace inet;

Define_Module(HttpClientEvilOperator);

void HttpClientEvilOperator::initialize(int stage) {
    TcpAppBase::initialize(stage);

    if(stage == INITSTAGE_LOCAL) {
        const char* strResPubSig = "httpAtkResSig";
        const char* strMsgPubSig = "httpAtkMsgSig";
        // Initializing inherited signals
        resPubSig = registerSignal(strResPubSig);
        msgPubSig = registerSignal(strMsgPubSig);
        cmdListener = new OpListener(this);
        const char* strCliCmdSig = "httpAtkCmdSig";
        // Go up of two levels in the modules hierarchy (the first is the host module)
        getParentModule()->getParentModule()->subscribe(strCliCmdSig, cmdListener);
    }
}
