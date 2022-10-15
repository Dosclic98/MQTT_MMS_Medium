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

package tx_medium_exp.lan.wireless;

import inet.node.wireless.AccessPoint;
import inet.node.inet.WirelessHost;
import tx_medium_exp.lan.ICliLan;

module WirelessCliLan like ICliLan {
    parameters:
        int numGoodClient = default(1);
        int numBadClient = default(1);

    gates:
        inout gate;

    submodules:
        client[numGoodClient + numBadClient]: WirelessHost {
            @display("p=106,157");
        }
        accessPoint: AccessPoint {
            @display("p=258,156");
        }
        
    connections:
        accessPoint.ethg++ <--> gate;
}
