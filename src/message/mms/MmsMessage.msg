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

//
// MESSAGEKIND CLIENT
// SEND: [0 listen to measures (register on server)] [2 generic request]
// MESSAGEKIND SERVER 
// SEND [1 send data to listeners] 
// RESPOND [3 generic response]
//
//GENERAL
//[999 I AM THE SERVER]
enum MMSKind {
    // Value used by the BadMmsClient
    FAKE = -1;
    
	CONNECT = 0;
	MEASURE = 1;
	GENREQ = 2;
	GENRESP = 3;
	DISCONNECT = 4;
	// TODO this is just used for the old MMS implementation
	SERVER = 99;
}

enum ReqResKind {
    UNSET = -1;
    READ = 0;
	COMMAND = 1;
}

// Expresses the 
enum MITMKind {
	UNMOD = 0;
	COMPR = 1;
	BLOCK = 2;
	FAKEGEN = 3;
}

class MmsMessage extends FieldsChunk
{
    // Id to keep track of the request associated with a certain response (just used in the MITM attack)
    msgid_t originId;
    B expectedReplyLength;         
    bool serverClose;      
    MMSKind messageKind;
    // Just set if the messageKind is set o GENREQ or GENRESP
    ReqResKind reqResKind;
    // Mantained for backward compatibility with the old implementation
    int connId;
    // Used just for the MMS MIMT Attack
    int evilServerConnId = 0;
    int serverIndex = 0;
    MITMKind atkStatus = MITMKind::UNMOD;
    // Data that defines the message data (0-9)
    int data = 0;
}
