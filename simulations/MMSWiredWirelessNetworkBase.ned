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

network MMSWiredWirelessNetworkBase
{
    parameters:
        int numWiredClients = default(1);
        int numWirelessClients = default(1);

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
        wiredClient[numWiredClients]: StandardHost;
        wirelessClient[numWirelessClients]: WirelessHost;
        evilClient: StandardHost {
            @display("p=374,57");
        }
        server: StandardHost {
            @display("p=589,142");
        }
        accessPoint: AccessPoint {
            @display("p=364,235");
        }
        router: Router {
            @display("p=374,142");
        }
        switch: EthernetSwitch {
            @display("p=250,141");
        }

    connections:
        switch.ethg++ <--> Eth1G <--> router.ethg++;
        router.pppg++ <--> {  datarate = 1Gbps; } <--> server.pppg++;
        evilClient.ethg++ <--> Eth1G <--> router.ethg++;
        accessPoint.ethg++ <--> Eth1G <--> router.ethg++;
        for i=0..numWiredClients-1 {
            wiredClient[i].ethg++ <--> Eth1G <--> switch.ethg++;
        }
}
