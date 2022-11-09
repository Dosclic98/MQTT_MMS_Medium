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

network MMSWirelessNetworkMITMBase {
    parameters:
        int numClients = default(1);
        int numServers = default(1);
        
        @statistic[measureBlockGlobal](source=measureBlockSignal; record=count);
        @statistic[measureCompromisedGlobal](source=measureCompromisedSignal; record=count);
        
        @statistic[genericRequestBlockGlobal](source=genericRequestBlockSignal; record=count);
        @statistic[genericRequestCompromisedGlobal](source=genericRequestCompromisedSignal; record=count);
        
        @statistic[genericResponseBlockGlobal](source=genericResponseBlockSignal; record=count);
        @statistic[genericResponseCompromisedGlobal](source=genericResponseCompromisedSignal; record=count);
    
	submodules:
	    visualizer: IntegratedCanvasVisualizer { @display("p=450,50"); }
	    configurator: Ipv4NetworkConfigurator;
	    radioMedium: Ieee80211RadioMedium { @display("p=300,50"); }
	    client[numClients]: WirelessHost { 
	        @display("p=200,200");
	        numApps = parent.numServers;
	    }
	    evilClient: WirelessHost { 
	        @display("p=200,200"); 
	   		numApps = parent.numServers + 1;
	   	}
	    server[numServers]: WirelessHost { @display("p=600,215"); }
	    accessPoint: AccessPoint { @display("p=374,215"); }
}