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
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.node.ethernet.Eth1G;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;

network IncrementalTest {
	submodules:
	    configurator: Ipv4NetworkConfigurator;
	    client: MmsWiredClient{
	    	clientOperator.numApps = 1;
	    };
	    server: StandardHost {
	    	numApps = 1;
	    }
	    router: Router;
	    
	connections:
	    router.ethg++ <--> Eth1G <--> client.gate;
	    router.ethg++ <--> Eth1G <--> server.ethg++;
	    
}