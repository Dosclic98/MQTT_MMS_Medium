

package tx_medium_exp.server.mms;
import inet.applications.contract.IApp;

simple EvilServer like IApp
{
    parameters:
        @class("inet::EvilServer");
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
        
        volatile int serviceTime @unit(ms) = default(intuniform(10ms, 18ms));
        
        @signal[measureBlockSignal](type=bool);
        @statistic[measureBlock](source=measureBlockSignal; record=count);
        @signal[measureCompromisedSignal](type=bool);
        @statistic[measureCompromised](source=measureCompromisedSignal; record=count);
        
        @signal[genericRequestBlockSignal](type=bool);
        @statistic[genericRequestBlock](source=genericRequestBlockSignal; record=count);
        @signal[genericRequestCompromisedSignal](type=bool);
        @statistic[genericRequestCompromised](source=genericRequestCompromisedSignal; record=count);
        
        @signal[genericResponseBlockSignal](type=bool);
        @statistic[genericResponseBlock](source=genericResponseBlockSignal; record=count);
        @signal[genericResponseCompromisedSignal](type=bool);
        @statistic[genericResponseCompromised](source=genericResponseCompromisedSignal; record=count);
    gates:
        input socketIn @labels(TcpCommand/up);
        output socketOut @labels(TcpCommand/down);
}
