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

package tx_medium_exp.operator.client;

import inet.applications.contract.IApp;

simple HttpClientOperator like IApp {
     parameters:
     	@class("inet::HttpClientOperator");
     	
     	@display("i=block/app");
     	string localAddress = default(""); // may be left empty ("")
        int localPort = default(-1); // port number to listen on
        int connectPort = default(443); // port number to connect to
     	
     	@signal[httpMsgSig]();
		@signal[httpResSig]();
		@signal[packetSent](type=inet::Packet);
     gates:
        input socketIn @labels(TcpCommand/up);
        output socketOut @labels(TcpCommand/down);
}
