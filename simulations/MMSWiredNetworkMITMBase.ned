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

import inet.node.inet.StandardHost;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.ethernet.*;

network MMSWiredNetworkMITMBase {
	parameters:
		int numClients = default(3);
		int numServers = default(1);
		
	submodules:
	    configurator: Ipv4NetworkConfigurator;
	    client[numClients]: StandardHost;
	    evilClient: StandardHost;
	    server[numServers]: StandardHost;
        router: Router {
            @display("p=157,128");
        }
        switch: EthernetSwitch {
            @display("p=157,213");
        }
    
    connections:
        switch.ethg++ <--> Eth1G <--> router.ethg++;
        evilClient.ethg++ <--> Eth1G <--> router.ethg++;
        for i = 0..numClients-1 {
        	client[i].ethg++ <--> Eth1G <--> switch.ethg++;
        }
        for j = 0..numServers-1 {
        	router.pppg++ <--> { datarate = 1Gbps; } <--> server[j].pppg++;
        }
	
}