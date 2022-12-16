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

package tx_medium_exp.lan.mobile;

import inet.node.inet.StandardHost;
import inet.node.inet.Router;
import inet.node.ethernet.*;
import simu5g.nodes.NR.NRUe;
import tx_medium_exp.lan.IDerLan;
import simu5g.nodes.NR.gNodeB;
import simu5g.nodes.Upf;

module Wireless5GDerLan like IDerLan
{
    parameters:
        int numServers = default(1);
    gates:
        inout gate;

    submodules:
        server[numServers]: NRUe {
            @display("i=misc/windturbine;p=524,85");
        }
        antenna: gNodeB {
            @display("p=456,157");
        }
        upf: Upf;
    connections:
        antenna.ppp <--> Eth1G <--> upf.pppg++;
        upf.filterGate <--> gate;

}
