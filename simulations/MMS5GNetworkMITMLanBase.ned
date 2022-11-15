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

import inet.node.inet.Router;
import tx_medium_exp.lan.wired.WiredCliLan;
import tx_medium_exp.lan.wired.WiredDerLan;
import inet.node.ethernet.*;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import tx_medium_exp.lan.ICliLan;
import tx_medium_exp.lan.IDerLan;
import inet.physicallayer.wireless.ieee80211.packetlevel.Ieee80211RadioMedium;
import simu5g.common.binder.Binder;
import simu5g.world.radio.LteChannelControl;
import simu5g.common.carrierAggregation.CarrierAggregation;
import simu5g.common.binder.Binder;


network MMS5GNetworkMITMLanBase{
    parameters:
        volatile int numDerLan = default(1);
        @statistic[measureBlockGlobal](source=measureBlockSignal; record=count);
        @statistic[measureCompromisedGlobal](source=measureCompromisedSignal; record=count);

        @statistic[genericRequestBlockGlobal](source=genericRequestBlockSignal; record=count);
        @statistic[genericRequestCompromisedGlobal](source=genericRequestCompromisedSignal; record=count);

        @statistic[genericResponseBlockGlobal](source=genericResponseBlockSignal; record=count);
        @statistic[genericResponseCompromisedGlobal](source=genericResponseCompromisedSignal; record=count);

    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=51,168");
        }

        carrierAggregation: CarrierAggregation {
            @display("p=142,100;is=s");
        }
        channelControl: LteChannelControl {
            @display("p=50,25;is=s");
        }
        radioMedium: Ieee80211RadioMedium {
            @display("p=51,88");
        }
        router: Router {
            @display("i=abstract/router2");
        }
        goodCliLan: <> like ICliLan {
            @display("i=misc/house");
        }
        derLan[numDerLan]: <> like IDerLan {
            @display("i=device/lan");
        }
        binder: Binder {
            @display("p=35,38");
        }

    connections:
        goodCliLan.gate <--> Eth1G <--> router.ethg++;
        for i=0..(numDerLan-1) {
            derLan[i].gate <--> Eth1G <--> router.ethg++;
        }

}
