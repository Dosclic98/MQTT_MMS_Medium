

package tx_medium_exp.client.mms;

import inet.applications.contract.IApp;

simple MmsClient like IApp
{
    parameters:
        @class("inet::MmsClient");
        string localAddress = default(""); // may be left empty ("")
        int localPort = default(-1); // port number to listen on
        string connectAddress = default("");  // server address (may be symbolic)
        int connectPort = default(1000); // port number to connect to
        double startTime @unit(s) = default(1s); // time first session begins
        double stopTime @unit(s) = default(-1s);  // time of finishing sending, negative values mean forever
        volatile int numRequestsPerSession = default(1);  // number of requests sent per session
        volatile int requestLength @unit(B) = default(200B); // length of a request
        volatile int replyLength @unit(B) = default(1MiB); // length of a reply
        volatile int thinkTime @unit(ms); // time gap between requests
        volatile double idleInterval @unit(s); // time gap between sessions
        volatile double reconnectInterval @unit(s) = default(30s);  // if connection breaks, waits this much before trying to reconnect
        int timeToLive = default(-1); // if not -1, set the TTL (IPv4) or Hop Limit (IPv6) field of sent packets to this value
        int dscp = default(-1); // if not -1, set the DSCP (IPv4/IPv6) field of sent packets to this value
        int tos = default(-1); // if not -1, set the Type Of Service (IPv4) / Traffic Class (IPv6) field of sent packets to this value
        @display("i=block/app");
        @lifecycleSupport;
        double stopOperationExtraTime @unit(s) = default(-1s);    // extra time after lifecycle stop operation finished
        double stopOperationTimeout @unit(s) = default(2s);    // timeout value for lifecycle stop operation
        volatile int resTimeoutInterval @unit(s) = default(1s);		// timeout for generic responses
        bool isLogging = default(false);	// specify if the specific client must Log the packets on file
        volatile int sendReadInterval @unit(s) = default(intuniform(8s, 10s));	// specify the interval between a read request and the next 
        volatile int sendCommandInterval @unit(s) = default(intuniform(12s, 15s));	// specify the interval between a command request and the next
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
        
        @signal[genericResponseSignal](type=bool);
        @statistic[genericResponse](source=genericResponseSignal; record=count);
        
        @signal[genericResponseSignal](type=bool);
        @statistic[genericResponse](source=genericResponseSignal; record=count);
        
        @signal[genericResponseTimeoutSignal](type=bool);
        @statistic[genericResponseTimeout](source=genericResponseTimeoutSignal; record=count);
    gates:
        input socketIn @labels(TcpCommand/up);
        output socketOut @labels(TcpCommand/down);
}

