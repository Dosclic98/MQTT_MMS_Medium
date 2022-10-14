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

import inet.node.inet.Router;
import tx_medium_exp.lan.CliLan;
import tx_medium_exp.lan.DerLan;
import inet.node.ethernet.*;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;


network MMSWiredNetworkDosLanBase {
	
	submodules:
	    configurator: Ipv4NetworkConfigurator;
	    router: Router;
	    goodCliLan: CliLan;
	    badCliLan: CliLan;
	    derLan: DerLan;
	
	connections:
	    goodCliLan.gate <--> Eth1G <--> router.ethg++;
	    badCliLan.gate <--> Eth1G <--> router.ethg++;
	    derLan.gate <--> Eth1G <--> router.ethg++;
	   	
}