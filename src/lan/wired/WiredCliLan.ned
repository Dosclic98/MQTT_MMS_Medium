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

package tx_medium_exp.lan.wired;

import inet.node.inet.StandardHost;
import inet.node.inet.Router;
import inet.node.ethernet.*;
import tx_medium_exp.lan.ICliLan;


module WiredCliLan like ICliLan {

    parameters:
        int numGoodClient = default(1);
        int numBadClient = default(1);

    gates:
        inout gate;

    submodules:
        client[numGoodClient + numBadClient]: StandardHost;
        router: Router {
            @display("p=476,289");
        }
        switch: EthernetSwitch {
            @display("p=343,288");
        }
	
	connections:
	    switch.ethg++ <--> Eth1G <--> router.ethg++;
	    router.ethg++ <--> gate;
	    for i=0..(numGoodClient+numBadClient)-1{
	    	client[i].ethg++ <--> Eth1G <--> switch.ethg++;
	    }
}
