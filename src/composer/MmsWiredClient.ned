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

import tx_medium_exp.operator.client.MmsClientOperator;
import tx_medium_exp.controller.client.MmsClientController;
import inet.node.inet.StandardHost;

module MmsWiredClient {
    parameters:
        volatile int numServers = default(1);
    
   	gates:
        inout gate;    
        
    submodules:
        clientOperator: StandardHost {
        	numApps = parent.numServers;
        };
    	clientController[numServers]: MmsClientController;   
    
    connections:
        clientOperator.ethg++ <--> gate; 
   	    
}
