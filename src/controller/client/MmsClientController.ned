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

package tx_medium_exp.controller.client;

simple MmsClientController {
	parameters:
	    @class("inet::MmsClientController");
	    @display("i=block/cogwheel");
	    
	    volatile int sendReadInterval @unit(ms) = default(intuniform(8000ms, 10000ms));	// specify the interval between a read request and the next 
        volatile int sendCommandInterval @unit(ms) = default(intuniform(12000ms, 15000ms));	// specify the interval between a command request and the next
		
		@signal[cliMsgSig-*]();
	    @signal[cliCmdSig-*]();
}