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

package tx_medium_exp.controller.attacker;

import tx_medium_exp.controller.client.HttpClientController;


simple HttpAttackerController extends HttpClientController {
    
    parameters:
		@class("inet::HttpAttackerController");
	
	    string netIpPrefix = "192.168.1";
     	int maxNetSpace = default(256);
    
		
		@signal[httpAtkCmdSig]();
		@signal[httpAtkMsgSig]();
}
