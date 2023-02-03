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

package tx_medium_exp.server.mms.multi;

import inet.applications.tcpapp.TcpGenericServerApp;

//
// TODO auto-generated module
//
simple ServerEvilComp extends TcpGenericServerApp {
    parameters:
        @class("inet::ServerEvilComp");
        
        bool isLogging = default(false); // For logging purposes
        bool startFull = default(false); // Makes the ServerComp start and stay in "Full" state
        volatile int serviceTime @unit(ms) = default(intuniform(10ms, 18ms));
        volatile int forwardDelay @unit(us) = default(intuniform(1us, 3us));
        volatile double stateChangeDelay @unit(s) = default(erlang_k(4, 30s));
        
        @signal[pcktFromClientSignal-*](type=Packet);
}
