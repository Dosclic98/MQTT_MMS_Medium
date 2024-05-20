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
import tx_medium_exp.composer.MmsWiredAttacker;
import tx_medium_exp.composer.HistorianServer;
import tx_medium_exp.graph.attack.AttackGraph;
import inet.node.inet.Router;
import inet.node.ethernet.EthernetSwitch;
import inet.node.inet.StandardHost;
import inet.node.ethernet.Eth1G;
import inet.node.ethernet.Eth100M;
import tx_medium_exp.binder.ControllerBinder;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;

network IncrementalTest
{
    parameters:
        volatile int numMmsServers = default(1);
        @display("bgb=633,673");
    submodules:
        attackGraph: AttackGraph {
            @display("p=54,121");
        }
        controllerBinder: ControllerBinder {
            @display("p=54,38");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=54,204");
        }
        stationComputer: StationComputer {
            numMmsServers = parent.numMmsServers;
            @display("p=78,514");
        }
        server[numMmsServers]: MmsWiredServer;
        attacker: MmsWiredAttacker {
            numMmsServers = parent.numMmsServers;
            @display("p=474,55");
        }
        pkiServer: StandardHost {
            @display("p=474,191;i=device/server");
        }
        historianServer: HistorianServer {
            @display("p=474,293");
        }
        // Network components
        itSwitch: EthernetSwitch {
            @display("p=283,54");
        }
        router1: Router {
            @display("p=283,157");
        }
        dmzSwitch: EthernetSwitch {
            @display("p=283,261");
        }
        router2: Router {
            @display("p=283,378");
        }
        otSwitch: EthernetSwitch {
            @display("p=283,513");
        }
        router3: Router {
            @display("p=437,514");
        }
    connections:
        attacker.gate <--> Eth1G <--> itSwitch.ethg++;
        itSwitch.ethg++ <--> Eth1G <--> router1.ethg++;
        router1.ethg++ <--> Eth1G <--> dmzSwitch.ethg++;
        pkiServer.ethg++ <--> Eth100M <--> dmzSwitch.ethg++;
        historianServer.gate <--> Eth100M <--> dmzSwitch.ethg++;
        dmzSwitch.ethg++ <--> Eth1G <--> router2.ethg++;
        router2.ethg++ <--> Eth1G <--> otSwitch.ethg++;
        stationComputer.gate <--> Eth1G <--> otSwitch.ethg++;
        otSwitch.ethg++ <--> Eth1G <--> router3.ethg++;
        for i=0..numMmsServers-1 {
            router3.ethg++ <--> Eth1G <--> server[i].gate;
        }

}
