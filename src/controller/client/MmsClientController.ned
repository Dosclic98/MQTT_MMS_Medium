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
	    volatile int sendReadInterval @unit(s) = default(intuniform(8s, 10s));	// specify the interval between a read request and the next 
        volatile int sendCommandInterval @unit(s) = default(intuniform(12s, 15s));	// specify the interval between a command request and the next
}