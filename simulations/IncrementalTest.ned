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

package tx_medium_exp.simulations;

import tx_medium_exp.composer.MmsWiredClient;
import tx_medium_exp.composer.MmsWiredServer;
import tx_medium_exp.composer.MmsWiredAttacker;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.node.ethernet.Eth1G;
import tx_medium_exp.binder.ControllerBinder;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;

network IncrementalTest {
    parameters:
        volatile int numServers = default(1);
	submodules:
	    controllerBinder: ControllerBinder;
	    configurator: Ipv4NetworkConfigurator;
	    client: MmsWiredClient {
	    	numServers = parent.numServers;
	    };
	    server[numServers]: MmsWiredServer;
	    attacker: MmsWiredAttacker {
	    	numServers = parent.numServers;
	    };
	    router: Router;
	    
	connections:
	    for i=0..numServers-1 {
	    	router.ethg++ <--> Eth1G <--> server[i].gate;
	    }
	    router.ethg++ <--> Eth1G <--> client.gate;
	    router.ethg++ <--> Eth1G <--> attacker.gate;
	    
}