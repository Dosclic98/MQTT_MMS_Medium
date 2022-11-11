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
import inet.visualizer.canvas.integrated.IntegratedCanvasVisualizer;
import inet.node.inet.StandardHost;

network MMSWiredWirelessNetworkDoSBase
{
    parameters:
        int numWiredGoodClient = default(3);
        int numWirelessBadClient = default(0);
        int numServers = default(1);

    submodules:
        visualizer: IntegratedCanvasVisualizer {
            @display("p=130,33");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=211,33");
        }
        radioMedium: Ieee80211RadioMedium {
            @display("p=49,32");
        }
        wiredClient[numWiredGoodClient]: StandardHost;
        wirelessClient[numWirelessBadClient]: WirelessHost;
        server[numServers]: StandardHost {
        	@display("i=misc/windturbine");
        };
        accessPoint: AccessPoint {
            @display("p=359,240");
        }
        router: Router {
            @display("p=500,142");
        }
        switch: EthernetSwitch {
            @display("p=250,141");
        }

    connections:
        switch.ethg++ <--> Eth1G <--> router.ethg++;
        accessPoint.ethg++ <--> Eth100M <--> router.ethg++;
        for j=0..(numServers-1) {
            router.ethg++ <--> Eth100M <--> server[j].ethg++;
        }
        for i=0..numWiredGoodClient-1 {
            wiredClient[i].ethg++ <--> Eth100M <--> switch.ethg++;
        }
}
