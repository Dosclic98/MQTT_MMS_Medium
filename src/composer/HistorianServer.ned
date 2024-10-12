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

package tx_medium_exp.composer;

import tx_medium_exp.operator.server.HttpServerOperator;
import tx_medium_exp.controller.server.HttpServerController;
import tx_medium_exp.controller.client.HttpClientController;
import inet.node.inet.StandardHost;


module HistorianServer {
	parameters:
        @display("i=device/server");
    
    gates:
        inout gate;
        
    submodules:
        historianHostOperator: StandardHost {
        	numApps = 2;
        };
        
    	httpServerController: HttpServerController;   
    	httpClientController: HttpClientController;
    	
    connections:
        historianHostOperator.ethg++ <--> gate;
        
}