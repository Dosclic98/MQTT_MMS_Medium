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
import simu5g.common.binder.Binder;
import simu5g.common.carrierAggregation.CarrierAggregation;
import simu5g.nodes.NR.NRUe;
import simu5g.nodes.NR.gNodeB;
import simu5g.nodes.Upf;
import simu5g.world.radio.LteChannelControl;


network MMSWired5GNetworkDoSBase
{
    parameters:
        int numGoodClient = default(1);
        int numBadClient = default(1);

        int numServers = default(1);

    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=467,256");
        }
        binder: Binder {
            @display("p=467,105");
        }
        carrierAggregation: CarrierAggregation {
            @display("p=467,177;is=s");
        }
        channelControl: LteChannelControl {
            @display("p=466,33;is=s");
        }
        upf: Upf {
            @display("p=157,198");
        }
        client[numGoodClient + numBadClient]: StandardHost;
        server[numServers]: NRUe {
            @display("i=misc/windturbine;p=157,33");
        }
        internetCloud: InternetCloud {
            @display("p=156,267");
        }
        switch: EthernetSwitch {
            @display("p=157,339");
        }
        gnb1: gNodeB {
            @display("p=86,137");
        }
        gnb2: gNodeB {
            @display("p=231,137");
        }

    connections:
        switch.ethg++ <--> Eth1G <--> internetCloud.ethg++;
        internetCloud.pppg++ <--> Eth1G <--> upf.filterGate;
        gnb1.ppp <--> Eth1G <--> upf.pppg++;
        gnb2.ppp <--> Eth1G <--> upf.pppg++;

        for i=0..(numGoodClient + numBadClient)-1 {
            client[i].ethg++ <--> Eth100M <--> switch.ethg++;
        }

}
