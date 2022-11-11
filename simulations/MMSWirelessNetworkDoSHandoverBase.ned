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

import inet.node.inet.WirelessHost;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.wireless.AccessPoint;
import inet.physicallayer.wireless.ieee80211.packetlevel.Ieee80211RadioMedium;
import inet.node.ethernet.*;
import inet.physicallayer.wireless.noise.NoiseSource;
import inet.node.inet.StandardHost;
import inet.visualizer.canvas.integrated.IntegratedCanvasVisualizer;

network MMSWirelessNetworkDoSHandoverBase
{
    parameters:
        int numGoodClient = default(3);
        int numBadClient = default(0);
        int numServers = default(1);

    submodules:
        visualizer: IntegratedCanvasVisualizer {
            @display("p=208,33");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=37,33");
        }
        radioMedium: Ieee80211RadioMedium {
            @display("p=121,32");
        }
        client[numGoodClient + numBadClient]: WirelessHost {
            @display("p=199,224");
        }
        server[numServers]: StandardHost {
            @display("i=misc/windturbine;p=315,635");
        }
        accessPoint1: AccessPoint {
            @display("p=130,385");
        }
        accessPoint2: AccessPoint {
            @display("p=503,385");
        }
        router: Router {
            @display("p=315,519");
        }
        
    connections:
        accessPoint1.ethg++ <--> Eth1G <--> router.ethg++;
        accessPoint2.ethg++ <--> Eth1G <--> router.ethg++;
        for j = 0..(numServers-1) {
        	router.ethg++ <--> Eth100M <--> server[j].ethg++;
        }
        
}
