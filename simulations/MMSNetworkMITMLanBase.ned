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
import inet.node.internetcloud.InternetCloud;


network MMSNetworkMITMLanBase
{
    parameters:
        volatile int numDerLan = default(1);
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
        radioMedium: Ieee80211RadioMedium {
            @display("p=140,200");
        }
        internetCloud: InternetCloud;
        goodCliLan: <> like ICliLan {
        	@display("i=misc/house");
        };
        derLan[numDerLan]: <> like IDerLan {
        	@display("i=device/lan");
        };
        
    connections:
        goodCliLan.gate <--> Eth1G <--> internetCloud.ethg++;
        for i=0..(numDerLan-1) {
            derLan[i].gate <--> Eth1G <--> internetCloud.ethg++;	
        }

}
