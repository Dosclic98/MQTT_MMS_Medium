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

#ifndef __MQTT_TCP_MMSSERVER_H_
#define __MQTT_TCP_MMSSERVER_H_


#include "inet/applications/tcpapp/TcpGenericServerApp.h"
#include "inet/common/packet/ChunkQueue.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "../../message/mms/MmsMessage_m.h"
#include "../../utils/logger/server/MmsServerPacketLogger.h"

namespace inet {

/**
 * Generic server application. It serves requests coming in GenericAppMsg
 * request messages. Clients are usually subclassed from TcpAppBase.
 *
 * @see GenericAppMsg, TcpAppBase
 */

enum ServerOp {
	STABLE = 0,
	UNSTABLE = 1
};

class MmsServer : public TcpGenericServerApp
{

  protected:
    std::list< std::pair<int,int> > clientConnIdList;

    //Gestione servitore e coda
    bool serverStatus;
    cQueue serverQueue;
    cMessage* departureEvent;

    // Unstable prob params
    int q;
    int k;

    // Status vars
    ServerOp serverOp;
    int numComprMsgs;

    //Gestione invio misure
    cMessage* sendDataEvent;

    // Logging pacchetti inviati/ricevuti
    bool isLogging;
    MmsServerPacketLogger* logger;

  protected:
    virtual void sendBack(cMessage *msg) override;

    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    //--
    virtual void handleDeparture();
    virtual void sendPacketDeparture(int connId, msgid_t originId, int evilConnId, B requestedBytes, B replyLength,
    		MMSKind messageKind, ReqResKind reqResKind, int data, MITMKind atkStatus);

    void logPacket(Packet* packet, ServerOp serverOp);
    void updateOp(int numComprMsg);

  public:
    virtual ~MmsServer();
};

} // namespace inet


#endif

