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
// TOPICS: [0 report] [1 command] 
//
// MESSAGEKIND [0 publish] [1 subscription] [2 unsubscription]
//
class MqttMessage extends FieldsChunk
{
    B expectedReplyLength;         
    bool serverClose;        // with TCP: if true, server should close the connection
    int messageKind;
    int topic;
}
