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

import inet.applications.tcpapp.TcpBasicClientApp;

//
// TODO auto-generated module
//
simple ClientEvilComp extends TcpBasicClientApp {
	parameters:
	    @class("inet::ClientEvilComp");
	    
	    volatile int fakeGenReqThresh = default(10);
	    
        @signal[genericFakeReqResSignal](type=bool);
        @statistic[genericFakeReqRes](source=genericFakeReqResSignal; record=count);
	    
	    @signal[topicAmount](type=int);
        @statistic[topicAmountStat](source=topicAmount; record=vector);
        @signal[genericResponseSignal](type=bool);
        @statistic[genericResponse](source=genericResponseSignal; record=count);
        
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
		
		@signal[pcktFromServerSignal](type=cObject*);
}
