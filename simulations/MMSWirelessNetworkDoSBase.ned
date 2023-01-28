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
import inet.node.internetcloud.InternetCloud;

network MMSWirelessNetworkDoSBase
{
    parameters:
        int numGoodClient = default(3);
        int numBadClient = default(0);

        int numServers = default(1);

    submodules:
        visualizer: IntegratedCanvasVisualizer {
            @display("p=450,50");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=592,51");
        }
        radioMedium: Ieee80211RadioMedium {
            @display("p=300,50");
        }
        client[numGoodClient + numBadClient]: WirelessHost {
            @display("p=156,216");
        }
        server[numServers]: WirelessHost {
            @display("i=misc/windturbine;p=860,216");
        }
        clientAP: AccessPoint {
            @display("p=354,215");
        }
        serverAP: AccessPoint {
            @display("p=690,215");
        }
        internetCloud: InternetCloud {
            @display("p=523,216");
        }
	
	connections:
	    clientAP.ethg++ <--> Eth1G <--> internetCloud.ethg++;
	    serverAP.ethg++ <--> Eth1G <--> internetCloud.ethg++;
}
