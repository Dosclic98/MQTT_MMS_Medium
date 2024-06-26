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

import inet.node.inet.StandardHost;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.ethernet.*;
import inet.node.internetcloud.InternetCloud;
import inet.common.scenario.ScenarioManager;

network MMSWiredNetworkMITMBase {
	parameters:
		int numClients = default(3);
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
	    configurator: Ipv4NetworkConfigurator;
	    scenarioManager: ScenarioManager;
	    client[numClients]: StandardHost {
	    	numApps = parent.numServers;
	    };
	    evilClient: StandardHost {
	        @display("i=old/comp_a");
	    	numApps = parent.numServers + 1;
	    };
	    server[numServers]: StandardHost {
	    	@display("i=misc/windturbine");
	    };
        internetCloud: InternetCloud;
        router[3]: Router;
        switch: EthernetSwitch;
    
    connections:
        switch.ethg++ <--> Eth1G <--> router[0].ethg++;
        // Router triangle
        router[0].ethg++ <--> Eth100M <--> router[1].ethg++;
        router[0].ethg++ <--> Eth1G <--> router[2].ethg++;
        router[1].ethg++ <--> Eth1G <--> internetCloud.ethg++;
        router[1].ethg++ <--> Eth1G <--> router[2].ethg++;
        
        evilClient.ethg++ <--> Eth100M <--> internetCloud.ethg++;
        for i = 0..numClients-1 {
        	client[i].ethg++ <--> Eth100M <--> switch.ethg++;
        }
        for j = 0..numServers-1 {
        	internetCloud.ethg++ <--> Eth100M <--> server[j].ethg++;
        }
	
}
