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

import inet.common.INETDefs;
import inet.common.Units;
import inet.common.packet.chunk.Chunk;

namespace inet;

class HttpBaseMessage extends FieldsChunk {
	int protocol = 11; 				// 10 for http/1.0, 11 for http/1.1.
	int contentLength;				// Length of the response body
	string contentType="text/text";	// Content-type
	string body="";					// The payload field
}

class HttpRequestMessage extends HttpBaseMessage {
    string method;					// The method used (e.g. GET, POST, PUT...)
	string targetUrl; 				// The target URL
	bool keepAlive = true;			// The keep-alive header
}

class HttpResponseMessage extends HttpBaseMessage {
	int result=0; 					// e.g. 200 for OK, 404 for NOT FOUND.
	string originatorUrl="";		// The originator URL. Only applicable for servers.
}
