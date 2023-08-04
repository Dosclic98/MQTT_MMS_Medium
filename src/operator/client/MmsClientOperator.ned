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

import inet.applications.tcpapp.TcpBasicClientApp;

simple MmsClientOperator extends TcpBasicClientApp {
    parameters:
        @class("inet::MmsClientOperator");
        volatile int resTimeoutInterval @unit(s) = default(1s);		// timeout for generic responses
        bool isLogging = default(false);	// specify if the specific client must Log the packets on file
        
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
        @signal[connect](type=long);  // 1 for open, -1 for close
        @statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[packetSent](title="packets sent"; source=packetSent; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[endToEndDelay](title="end-to-end delay"; source="dataAge(packetReceived)"; unit=s; record=histogram,weightedHistogram,vector; interpolationmode=none);
        @statistic[numActiveSessions](title="number of active sessions"; source="sum(connect)"; record=max,timeavg,vector; interpolationmode=sample-hold);
        @statistic[numSessions](title="total number of sessions"; source="sum(connect+1)/2"; record=last);
        
        @signal[measureReceivedCount](type=int);
        @statistic[measureReceivedAmount](source=measureReceivedCount; record=vector);
        
        @signal[readSentSignal](type=bool);
        @statistic[readSent](source=readSentSignal; record=count);
        
        @signal[commandSentSignal](type=bool);
        @statistic[commandSent](source=commandSentSignal; record=count);
        
        @signal[readResponseSignal](type=bool);
        @statistic[readResponse](source=readResponseSignal; record=count);
        
        @signal[commandResponseSignal](type=bool);
        @statistic[commandResponse](source=commandResponseSignal; record=count);
        
        @signal[readResponseTimeoutSignal](type=bool);
        @statistic[readResponseTimeout](source=readResponseTimeoutSignal; record=count);
        
        @signal[commandResponseTimeoutSignal](type=bool);
        @statistic[commandResponseTimeout](source=commandResponseTimeoutSignal; record=count);
        
        @signal[readResponseReceivedTimeSignal](type=int);
        @statistic[readResponseReceivedTime](source=readResponseReceivedTimeSignal; record=vector);
        
        @signal[commandResponseReceivedTimeSignal](type=int);
        @statistic[commandResponseReceivedTime](source=commandResponseReceivedTimeSignal; record=vector);    
}
