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

package tx_medium_exp.broker;
import inet.applications.contract.IApp;

//
// Generic server application for modelling TCP-based request-reply style
// protocols or applications.
//
// The module accepts any number of incoming TCP connections, and expects
// to receive messages of class ~GenericAppMsg on them. A message should
// contain how large the reply should be (number of bytes). ~TcpGenericServerApp
// will just change the length of the received message accordingly, and send
// back the same message object. The reply can be delayed by a constant time
// (replyDelay parameter).
//
// Compatible with both ~Ipv4 and ~Ipv6.
//
// @see ~GenericAppMsg, ~TcpBasicClientApp, ~TelnetApp
//
simple Broker like IApp
{
    parameters:
        @class(inet::Broker);
        string localAddress = default(""); // local address; may be left empty ("")
        int localPort = default(1000);     // localPort number to listen on
        volatile int replyDelay @unit(ms) = default(0ms);
        @display("i=block/app");
        @lifecycleSupport;
        double stopOperationExtraTime @unit(s) = default(-1s);    // extra time after lifecycle stop operation finished
        double stopOperationTimeout @unit(s) = default(2s);    // timeout value for lifecycle stop operation
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
        @statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[packetSent](title="packets sent"; source=packetSent; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[endToEndDelay](title="end-to-end delay"; source="dataAge(packetReceived)"; unit=s; record=histogram,weightedHistogram,vector; interpolationmode=none);
        // -- isDos & Service Time
        volatile int serviceTime @unit(ms) = default(intuniform(1ms, 3ms));
        bool isDos = default(false);
        //-- Topic Bloccati
        @signal[commandBlockedSignal](type=bool);
        @signal[reportBlockedSignal](type=bool);
        @statistic[commandBlocked](title="Command Blocked"; source="commandBlockedSignal"; record=count);
        @statistic[reportBlocked](title="Report Blocked"; source="reportBlockedSignal"; record=count);
        //-- Topic Compromessi
        @signal[commandCompromisedSignal](type=bool);
        @signal[reportCompromisedSignal](type=bool);
        @statistic[commandCompromised](title="Command Compromised"; source="commandCompromisedSignal"; record=count);
        @statistic[reportCompromised](title="Report Compromised"; source="reportCompromisedSignal"; record=count);
        //-- Topic Ricevuti
        @signal[reportCountSignal](type=bool);
        @signal[commandCountSignal](type=bool);
        @statistic[commandCountBroker](source="commandCountSignal"; record=count);
        @statistic[reportCountBroker](source="reportCountSignal"; record=count);
    gates:
        input socketIn @labels(TcpCommand/up);
        output socketOut @labels(TcpCommand/down);
}

