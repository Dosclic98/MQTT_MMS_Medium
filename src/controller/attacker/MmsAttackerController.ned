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

//
// TODO auto-generated module
//
simple MmsAttackerController {
	parameters:
	    @class("inet::MmsAttackerController");
	    
	   	volatile int fakeGenReqThresh = default(10);
	    
	    volatile double readResponseBlockProb = default(0.1);
	    volatile double readResponseCompromisedProb = default(0.6);
	    volatile double commandResponseBlockProb = default(0.1);
	    volatile double commandResponseCompromisedProb = default(0.6);
	    
	    volatile double readRequestBlockProb = default(0.2);
	    volatile double readRequestCompromisedProb = default(0.8);
	    volatile double commandRequestBlockProb = default(0.2);
	    volatile double commandRequestCompromisedProb = default(0.8);
	    	    
	    volatile double measureBlockProb = default(0.15);
	    volatile double measureCompromisedProb = default(0.4);
	    
	    
        @signal[genericFakeReqResSignal](type=bool);
        @statistic[genericFakeReqRes](source=genericFakeReqResSignal; record=count);
        
        @signal[measureBlockSignal](type=bool);
        @statistic[measureBlock](source=measureBlockSignal; record=count);
        @signal[measureCompromisedSignal](type=bool);
        @statistic[measureCompromised](source=measureCompromisedSignal; record=count);
        
        @signal[readRequestBlockSignal](type=bool);
        @statistic[readRequestBlock](source=readRequestBlockSignal; record=count);
        @signal[readRequestCompromisedSignal](type=bool);
        @statistic[readRequestCompromised](source=readRequestCompromisedSignal; record=count);
        
        @signal[commandRequestBlockSignal](type=bool);
        @statistic[commandRequestBlock](source=commandRequestBlockSignal; record=count);
        @signal[commandRequestCompromisedSignal](type=bool);
        @statistic[commandRequestCompromised](source=commandRequestCompromisedSignal; record=count);
        
        @signal[readResponseBlockSignal](type=bool);
        @statistic[readResponseBlock](source=readResponseBlockSignal; record=count);
        @signal[readResponseCompromisedSignal](type=bool);
        @statistic[readResponseCompromised](source=readResponseCompromisedSignal; record=count);
        
        @signal[commandResponseBlockSignal](type=bool);
        @statistic[commandResponseBlock](source=commandResponseBlockSignal; record=count);
        @signal[commandResponseCompromisedSignal](type=bool);
        @statistic[commandResponseCompromised](source=commandResponseCompromisedSignal; record=count);
}
