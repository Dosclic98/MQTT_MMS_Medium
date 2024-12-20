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

#include "MmsClientOperator.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/TimeTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "../../result/client/MmsClientResult.h"

#define MSGKIND_CONNECT    				0
#define MSGKIND_SEND_READ       		1
#define MSGKIND_SEND_COMMAND       		2
#define MSGKIND_RES_TIMEOUT	   			3

namespace inet {

Define_Module(MmsClientOperator);

MmsClientOperator::~MmsClientOperator() {
	if(isLogging) {
		delete logger;
	}
    cancelAndDelete(measureAmountEvent);
}

void MmsClientOperator::initialize(int stage) {
    TcpAppBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        numRequestsToSend = 0;
        WATCH(numRequestsToSend);
        WATCH(measureCounter);
        cEnvir* ev = getSimulation()->getActiveEnvir();
        // So it doesn't connect automatically
        startTime = SIMTIME_ZERO;
        stopTime = SIMTIME_ZERO;
        resTimeout = par("resTimeoutInterval");
        isLogging = par("isLogging");
        if(isLogging) {
        	logger = new MmsPacketLogger(ev->getConfigEx()->getActiveRunNumber(), "client", 0, getIndex());
        }
        timeoutMsg = new cMessage("timer");

        measureAmountEvent = new cMessage("Topic Amount Event");
        measureReceivedCount = registerSignal("measureReceivedCount");
        readSentSignal = registerSignal("readSentSignal");
        commandSentSignal = registerSignal("commandSentSignal");
        readResponseSignal = registerSignal("readResponseSignal");
        commandResponseSignal = registerSignal("commandResponseSignal");
        readResponseTimeoutSignal = registerSignal("readResponseTimeoutSignal");
        commandResponseTimeoutSignal = registerSignal("commandResponseTimeoutSignal");
        readResponseReceivedTimeSignal = registerSignal("readResponseReceivedTimeSignal");
        commandResponseReceivedTimeSignal = registerSignal("commandResponseReceivedTimeSignal");


        // Initializing inherited signals
        char strResPubSig[30];
        char strMsgPubSig[30];
        char strCliCmdSig[30];
        sprintf(strResPubSig, "cliResSig-%d", this->getIndex());
        sprintf(strMsgPubSig, "cliMsgSig-%d", this->getIndex());
        sprintf(strCliCmdSig, "cliCmdSig-%d", this->getIndex());

        resPubSig = registerSignal(strResPubSig);
        msgPubSig = registerSignal(strMsgPubSig);
        cmdListener = new OpListener(this);
        // Go up of two levels in the modules hierarchy (the first is the host module)
        getParentModule()->getParentModule()->subscribe(strCliCmdSig, cmdListener);

        measureCounter = 0;
        isListening = false;
        scheduleAt(simTime() + SimTime(measureAmountEventDelay, SIMTIME_S), measureAmountEvent);
    }
}

void MmsClientOperator::handleStartOperation(LifecycleOperation *operation) {/* Do nothing */}

void MmsClientOperator::socketClosed(TcpSocket *socket) {
    TcpAppBase::socketClosed(socket);
}


void MmsClientOperator::socketEstablished(TcpSocket *socket) {
    // determine number of requests in this session
    numRequestsToSend = par("numRequestsPerSession");
    if (numRequestsToSend < 1)
        numRequestsToSend = 1;
}

void MmsClientOperator::sendRequest(MMSKind kind, ReqResKind reqKind, int data) {
    long requestLength = par("requestLength");
    long replyLength = par("replyLength");
    if (requestLength < 1)
        requestLength = 1;
    if (replyLength < 1)
        replyLength = 1;

    bool setServerClosed = false;
    if(reqKind == ReqResKind::DISCONNECT) {
    	setServerClosed = true;
    }

    const auto& payload = makeShared<MmsMessage>();
    Packet *packet = new Packet("data");
    payload->setOriginId(packet->getId());
    payload->setChunkLength(B(requestLength));
    payload->setExpectedReplyLength(B(replyLength));
    payload->setServerClose(setServerClosed);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    payload->setServerIndex(this->getIndex());
    payload->setData(data);
    payload->setAtkStatus(MITMKind::UNMOD);
    EV << "Index: " << this->getIndex();
    if(!isListening && kind == MMSKind::CONNECT) {
    	// Connect kind
        payload->setMessageKind(kind);
        isListening = true;
    }
    else {
        payload->setMessageKind(kind);
        // Send a Read or a Command
        payload->setReqResKind(reqKind);

        // Add the timeout event for the respective response
        if(reqKind != ReqResKind::DISCONNECT) {
    		cMessage* resTimeoutMsg = new cMessage("Response timeout");
    		resTimeoutMsg->setKind(MSGKIND_RES_TIMEOUT);

            if(reqKind == ReqResKind::READ) {
            	readResTimeoutMap.insert({payload->getOriginId(), resTimeoutMsg});
            	emit(readSentSignal, true);
            } else if(reqKind == ReqResKind::COMMAND) {
            	commandResTimeoutMap.insert({payload->getOriginId(), resTimeoutMsg});
            	emit(commandSentSignal, true);
            }

            scheduleAt(simTime() + SimTime(resTimeout, SIMTIME_S), resTimeoutMsg);

            // Memorize when the message has been sent
            genReqSentTimeMap.insert({payload->getOriginId(), simTime()});
        }
    }

    packet->insertAtBack(payload);

    if(isLogging) logger->log(payload.get(), simTime());

    sendPacket(packet);
}

void MmsClientOperator::handleTimer(cMessage *msg)
{
    if (msg == measureAmountEvent) {
        emit(measureReceivedCount, measureCounter);
        measureCounter = 0;
        scheduleAt(simTime() + SimTime(measureAmountEventDelay, SIMTIME_S), measureAmountEvent);
        return;
    }
    bool found = false;
    switch (msg->getKind()) {
        case MSGKIND_RES_TIMEOUT:
        	for(auto &i : readResTimeoutMap) {
        		if (i.second == msg) {
        			readResTimeoutMap.erase(i.first);
        			genReqSentTimeMap.erase(i.first);
        			delete msg;
        			// Emit signal for generic response timeout
        			emit(readResponseTimeoutSignal, true);
        			found = true;
        			break;
        		}
        	}
        	if(!found) {
            	for(auto &i : commandResTimeoutMap) {
    				if (i.second == msg) {
    					commandResTimeoutMap.erase(i.first);
    					genReqSentTimeMap.erase(i.first);
    					delete msg;
    					// Emit signal for generic response timeout
    					emit(commandResponseTimeoutSignal, true);
    					break;
    				}
    			}
        	}

        	break;

        default:
            throw cRuntimeError("Invalid timer msg: kind=%d", msg->getKind());
    }
}


void MmsClientOperator::socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent)
{
    if (socket->getState() == TcpSocket::LOCALLY_CLOSED) {
        EV_INFO << "reply to last request arrived, closing session\n";
        close();
        return;
    }
    auto chunk = msg->peekDataAt(B(0), msg->getTotalLength());
    queue.push(chunk);
    while (const auto& appmsg = queue.pop<MmsMessage>(b(-1), Chunk::PF_ALLOW_NULLPTR)) {
    	if(isLogging) logger->log(const_cast<MmsMessage*>(appmsg.get()), simTime());
        if(appmsg->getMessageKind() == MMSKind::MEASURE) {
        	measureCounter++;
        }
        if(appmsg->getMessageKind() == MMSKind::GENRESP) {
        	simtime_t sendTime = genReqSentTimeMap.find(appmsg->getOriginId())->second;
        	if(appmsg->getReqResKind() == ReqResKind::READ) {
            	emit(readResponseSignal, true);
            	emit(readResponseReceivedTimeSignal, simTime() - sendTime);
            	if(readResTimeoutMap.find(appmsg->getOriginId()) != readResTimeoutMap.end()) {
            		cMessage* tmpTimeout = readResTimeoutMap[appmsg->getOriginId()];
            		readResTimeoutMap.erase(appmsg->getOriginId());
            		cancelAndDelete(tmpTimeout);
            	}
        	} else if(appmsg->getReqResKind() == ReqResKind::COMMAND) {
            	emit(commandResponseSignal, true);
            	emit(commandResponseReceivedTimeSignal, simTime() - sendTime);
            	if(commandResTimeoutMap.find(appmsg->getOriginId()) != commandResTimeoutMap.end()) {
            		cMessage* tmpTimeout = commandResTimeoutMap[appmsg->getOriginId()];
            		commandResTimeoutMap.erase(appmsg->getOriginId());
            		cancelAndDelete(tmpTimeout);
            	}
        	} else if(appmsg->getReqResKind() == ReqResKind::DISCONNECT) {
        		socket->close();
        	}
        	genReqSentTimeMap.erase(appmsg->getOriginId());
        }
    }
    TcpAppBase::socketDataArrived(socket, msg, urgent);
}

// Connect the TCP socket on the non-default connectAddress specified as parameter
void MmsClientOperator::connectWithAddress(std::string* connectAddress) {
	EV << "Connect address: " << connectAddress << "\n";
	// we need a new connId if this is not the first connection
	socket.renewSocket();

	const char *localAddress = par("localAddress");
	int localPort = par("localPort");
	socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);

	int timeToLive = par("timeToLive");
	if (timeToLive != -1)
		socket.setTimeToLive(timeToLive);

	int dscp = par("dscp");
	if (dscp != -1)
		socket.setDscp(dscp);

	int tos = par("tos");
	if (tos != -1)
		socket.setTos(tos);

	// connect
	int connectPort = par("connectPort");

	L3Address destination;
	L3AddressResolver().tryResolve(connectAddress->c_str(), destination);
	if (destination.isUnspecified()) {
		EV_ERROR << "Connecting to " << connectAddress << " port=" << connectPort << ": cannot resolve destination address\n";
	}
	else {
		EV_INFO << "Connecting to " << connectAddress << "(" << destination << ") port=" << connectPort << endl;

		socket.connect(destination, connectPort);

		numSessions++;
		emit(connectSignal, 1L);
	}
}

void MmsClientOperator::sendTcpConnect(int opId, std::string* connectAddress) {
	Enter_Method("Initializing TCP connection");
	if(connectAddress == nullptr) connect();
	else {
		connectWithAddress(connectAddress);
	}
	propagate(new MmsClientResult(opId, ResultOutcome::SUCCESS));
}

void MmsClientOperator::sendMmsConnect(int opId) {
	Enter_Method("Sending MMS Connect");
	if(socket.isOpen()) {
		sendRequest(MMSKind::CONNECT, ReqResKind::READ, 0);
		propagate(new MmsClientResult(opId, ResultOutcome::SUCCESS));
	} else {
		propagate(new MmsClientResult(opId, ResultOutcome::FAIL));
	}
}

// TODO Make this operation succeed if the disconnection packet is sent back
// (for now we consider the operation successfull if the disconncetion message is sent)
void MmsClientOperator::sendMmsDisconnect(int opId) {
	Enter_Method("Sending MMS Disconnect");
	if(socket.isOpen()) {
		sendRequest(MMSKind::GENREQ, ReqResKind::DISCONNECT);
		propagate(new MmsClientResult(opId, ResultOutcome::SUCCESS));
	}
}

void MmsClientOperator::sendMmsRequest(int opId, ReqResKind reqKind, int data) {
	Enter_Method("Sending MMS Request");
	if(socket.isOpen()) {
		sendRequest(MMSKind::GENREQ, reqKind, data);
		propagate(new MmsClientResult(opId, ResultOutcome::SUCCESS));
	} else {
		propagate(new MmsClientResult(opId, ResultOutcome::FAIL));
	}
}

void MmsClientOperator::propagate(IResult* res) {
	emit(this->resPubSig, res);
}

void MmsClientOperator::propagate(Packet* msg) {
	emit(this->msgPubSig, msg);
}



};
