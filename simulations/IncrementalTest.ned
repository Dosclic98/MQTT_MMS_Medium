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

import tx_medium_exp.composer.StationComputer;
import tx_medium_exp.composer.MmsWiredServer;
import tx_medium_exp.composer.Mms5GServer;
import tx_medium_exp.composer.MmsWiredAttacker;
import tx_medium_exp.composer.HistorianServer;
import tx_medium_exp.graph.attack.AttackGraph;
import inet.node.inet.Router;
import inet.node.ethernet.EthernetSwitch;
import inet.node.inet.StandardHost;
import inet.node.ethernet.Eth1G;
import inet.node.ethernet.Eth100M;
import simu5g.nodes.NR.gNodeB;
import simu5g.nodes.Upf;
import simu5g.world.radio.LteChannelControl;
import simu5g.common.carrierAggregation.CarrierAggregation;
import simu5g.common.binder.Binder;
import tx_medium_exp.binder.ControllerBinder;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;

network IncrementalTest
{
    parameters:
        volatile int numMmsServers = default(1);
        @display("bgb=1177,673");
    submodules:
        attackGraph: AttackGraph {
            @display("p=54,121");
        }
        controllerBinder: ControllerBinder {
            @display("p=54,38");
        }
        binder: Binder {

            @display("p=166,38");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=54,204");
        }
        carrierAggregation: CarrierAggregation {
            @display("p=54,378;is=s");
        }
        channelControl: LteChannelControl {
            @display("p=53,293;is=s");
        }
        stationComputer: StationComputer {
            numMmsServers = parent.numMmsServers;
            @display("p=157,514");
        }
        server[numMmsServers]: Mms5GServer {
            @display("p=907.89374,513.735");
        }
        workstation: MmsWiredAttacker {
            numMmsServers = parent.numMmsServers;
            @display("p=553,55");
        }
        pkiServer: StandardHost {
            @display("p=553,191;i=device/server");
        }
        historianServer: HistorianServer {
            @display("p=553,293");
        }
        // Network components
        itSwitch: EthernetSwitch {
            @display("p=362,54");
        }
        router1: Router {
            @display("p=362,157");
        }
        dmzSwitch: EthernetSwitch {
            @display("p=362,261");
        }
        router2: Router {
            @display("p=362,378");
        }
        otSwitch: EthernetSwitch {
            @display("p=362,513");
        }
        router3: Router {
            @display("p=507,514");
        }
        upf: Upf {
            @display("p=619,514");
        }
        gnb: gNodeB {
            @display("p=717,514");
        }

    connections:
        workstation.gate <--> Eth1G <--> itSwitch.ethg++;
        itSwitch.ethg++ <--> Eth1G <--> router1.ethg++;
        router1.ethg++ <--> Eth1G <--> dmzSwitch.ethg++;
        pkiServer.ethg++ <--> Eth100M <--> dmzSwitch.ethg++;
        historianServer.gate <--> Eth100M <--> dmzSwitch.ethg++;
        dmzSwitch.ethg++ <--> Eth1G <--> router2.ethg++;
        router2.ethg++ <--> Eth1G <--> otSwitch.ethg++;
        stationComputer.gate <--> Eth1G <--> otSwitch.ethg++;
        otSwitch.ethg++ <--> Eth1G <--> router3.ethg++;

		// Start of the 5G Network
        router3.pppg++ <--> Eth1G <--> upf.filterGate;
        upf.pppg++ <--> Eth1G <--> gnb.ppp;


}
