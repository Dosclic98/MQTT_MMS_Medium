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

package tx_medium_exp.composer;

import tx_medium_exp.operator.attacker.ClientEvilOperator;
import tx_medium_exp.operator.attacker.ServerEvilOperator;
import tx_medium_exp.controller.attacker.MmsAttackerController;
import tx_medium_exp.controller.attacker.HttpAttackerController;
import inet.node.inet.StandardHost;

module MmsWiredAttacker {
    parameters:
        @display("i=old/comp_a");
        volatile int numMmsServers = default(1);
    
    gates:
        inout gate;
        
    submodules:
        // As many apps as the number of servers plus the 
        // server itself plus the http client
        attackerOperator: StandardHost {
        	numApps = parent.numMmsServers + 2;
        };
        // As many controllers as the MMS client
    	attackerController[numMmsServers]: MmsAttackerController;   
    	// One controller more for the Http Client
    	httpAttackerController: HttpAttackerController;
    
    connections:
        attackerOperator.ethg++ <--> gate; 
}
