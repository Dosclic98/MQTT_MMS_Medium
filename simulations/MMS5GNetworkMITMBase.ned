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
import simu5g.common.binder.Binder;
import simu5g.common.carrierAggregation.CarrierAggregation;
import simu5g.nodes.NR.NRUe;
import simu5g.nodes.NR.gNodeB;
import simu5g.nodes.PgwStandard;
import simu5g.world.radio.LteChannelControl;

network MMS5GNetworkMITMBase
{
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
        binder: Binder;
        carrierAggregation: CarrierAggregation {
            @display("p=50.993748,258.7;is=s");
        }
        channelControl: LteChannelControl {
            @display("p=50,25;is=s");
        }
        // Must be called "upf" for the network to work correctly
        upf: PgwStandard {
            @display("p=157,54");
        }
        client[numClients]: StandardHost {
            numApps = parent.numServers;
        }
        evilClient: StandardHost {
            @display("i=old/comp_a;p=43,128");
            numApps = parent.numServers + 1;
        }
        server[numServers]: NRUe {
            @display("i=misc/windturbine;p=510,76");
        }
        antenna: gNodeB {
            @display("p=308,54");
        }
        router: Router {
            @display("p=157,128");
        }
        switch: EthernetSwitch {
            @display("p=157,213");
        }

    connections:
        switch.ethg++ <--> Eth1G <--> router.ethg++;
        evilClient.ethg++ <--> Eth100M <--> router.ethg++;
        antenna.ppp <--> Eth1G <--> upf.pppg++;
        router.pppg++ <--> Eth1G <--> upf.filterGate;
        for i=0..numClients-1 {
            client[i].ethg++ <--> Eth100M <--> switch.ethg++;
        }

}
