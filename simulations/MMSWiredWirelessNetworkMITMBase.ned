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
import inet.node.internetcloud.InternetCloud;

network MMSWiredWirelessNetworkMITMBase
{
    parameters:
        int numWiredClients = default(1);
        int numWirelessClients = default(1);
        int numServers = default(1);
        
        @statistic[measureBlockGlobal](source=measureBlockSignal; record=count);
        @statistic[measureCompromisedGlobal](source=measureCompromisedSignal; record=count);
        
        @statistic[readRequestBlockGlobal](source=readRequestBlockSignal; record=count);
        @statistic[readRequestCompromisedGlobal](source=readRequestCompromisedSignal; record=count);
        
        @statistic[commandRequestBlockGlobal](source=commandRequestBlockSignal; record=count);
        @statistic[commandRequestCompromisedGlobal](source=commandRequestCompromisedSignal; record=count);
        
        @statistic[readResponseBlockGlobal](source=readResponseBlockSignal; record=count);
        @statistic[readResponseCompromisedGlobal](source=readResponseCompromisedSignal; record=count);
        
        @statistic[commandResponseBlockGlobal](source=commandResponseBlockSignal; record=count);
        @statistic[commandResponseCompromisedGlobal](source=commandResponseCompromisedSignal; record=count);

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
        wiredClient[numWiredClients]: StandardHost {
        	numApps = parent.numServers;
        };
        wirelessClient[numWirelessClients]: WirelessHost {
        	numApps = parent.numServers;
        };
        evilClient: StandardHost {
            @display("p=374,57;i=old/comp_a");
            numApps = parent.numServers + 1;
        }
        server[numServers]: StandardHost {
            @display("p=589,142;i=misc/windturbine");
        }
        accessPoint: AccessPoint {
            @display("p=364,235");
        }
        internetCloud: InternetCloud {
            @display("p=374,142");
        }
        switch: EthernetSwitch {
            @display("p=250,141");
        }

    connections:
        switch.ethg++ <--> Eth1G <--> internetCloud.ethg++;
        evilClient.ethg++ <--> Eth100M <--> internetCloud.ethg++;
        accessPoint.ethg++ <--> Eth100M <--> internetCloud.ethg++;
        for i=0..numWiredClients-1 {
            wiredClient[i].ethg++ <--> Eth100M <--> switch.ethg++;
        }
        for j=0..numServers-1 {
        	internetCloud.ethg++ <--> Eth100M <--> server[j].ethg++;
        }
}
