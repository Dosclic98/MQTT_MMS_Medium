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
import simu5g.nodes.Ue;
import simu5g.nodes.eNodeB;
import simu5g.nodes.PgwStandard;
import simu5g.world.radio.LteChannelControl;

network MMS5GNetworkMITMBase
{
    parameters:
        int numClients = default(3);
        int numServers = default(1);

        @statistic[measureBlockGlobal](source=measureBlockSignal; record=count);
        @statistic[measureCompromisedGlobal](source=measureCompromisedSignal; record=count);

        @statistic[genericRequestBlockGlobal](source=genericRequestBlockSignal; record=count);
        @statistic[genericRequestCompromisedGlobal](source=genericRequestCompromisedSignal; record=count);

        @statistic[genericResponseBlockGlobal](source=genericResponseBlockSignal; record=count);
        @statistic[genericResponseCompromisedGlobal](source=genericResponseCompromisedSignal; record=count);

    submodules:
        configurator: Ipv4NetworkConfigurator;
        binder: Binder;
        carrierAggregation: CarrierAggregation {
            @display("p=50.993748,258.7;is=s");
        }
        channelControl: LteChannelControl {
            @display("p=50,25;is=s");
        }
        // Must be called "pgw" for the network to work correctly
        pgw: PgwStandard {
            @display("p=157,54");
        }
        client[numClients]: StandardHost {
            numApps = parent.numServers;
        }
        evilClient: StandardHost {
            @display("i=old/comp_a;p=43,128");
            numApps = parent.numServers + 1;
        }
        server[numServers]: Ue {
            @display("i=misc/windturbine;p=510,76");
        }
        antenna: eNodeB {
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
        antenna.ppp <--> Eth1G <--> pgw.pppg++;
        router.pppg++ <--> Eth1G <--> pgw.filterGate;
        for i=0..numClients-1 {
            client[i].ethg++ <--> Eth100M <--> switch.ethg++;
        }

}
