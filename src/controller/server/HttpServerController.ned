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

package tx_medium_exp.controller.server;


simple HttpServerController {
    parameters:
        @class("inet::HttpServerController");
	    @display("i=block/cogwheel");

        volatile int serviceTime @unit(ms) = default(intuniform(2ms, 4ms));
		
		@signal[httpSerMsgSig]();
	    @signal[httpSerCmdSig]();    
}
