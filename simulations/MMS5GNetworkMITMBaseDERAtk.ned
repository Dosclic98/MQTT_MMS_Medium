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
import inet.node.inet.WirelessHost;
import simu5g.common.binder.Binder;
import simu5g.common.carrierAggregation.CarrierAggregation;
import simu5g.nodes.NR.NRUe;
import simu5g.nodes.NR.gNodeB;
import simu5g.nodes.Upf;
import simu5g.world.radio.LteChannelControl;

network MMS5GNetworkMITMBaseDERAtk
{
    parameters:
        int numClients = default(1);
        int numServers = default(2);

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
        configurator: Ipv4NetworkConfigurator {
            @display("p=559,33");
        }
        binder: Binder {
            @display("p=227,33");
        }
        carrierAggregation: CarrierAggregation {
            @display("p=341,33;is=s");
        }
        channelControl: LteChannelControl {
            @display("p=466,33;is=s");
        }
        upf: Upf {
            @display("p=496,155");
        }
        switch: EthernetSwitch {
            @display("p=244,154");
        }
        client[numClients]: StandardHost {
            numApps = parent.numServers;
            @display("p=98,155");
        }
        evilClient: NRUe {
            @display("i=old/comp_a;p=741,27");
            numApps = parent.numServers + 1;
        }
        gnb1: gNodeB {
            @display("p=653,93");
        }
        gnb2: gNodeB {
            @display("p=653,228");
        }
        server[numServers]: NRUe {
            @display("i=misc/windturbine;p=812,155");
        }
        router: Router {
            @display("p=372,155");
        }

    connections:
        router.pppg++ <--> Eth1G <--> upf.filterGate;
        switch.ethg++ <--> Eth1G <--> router.ethg++;
        gnb1.ppp <--> Eth1G <--> upf.pppg++;
        gnb2.ppp <--> Eth1G <--> upf.pppg++;

        for i=0..numClients-1 {
            switch.ethg++ <--> Eth100M <--> client[i].ethg++;
        }
}
