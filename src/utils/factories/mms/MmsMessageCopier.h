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

#ifndef UTILS_FACTORIES_MMS_MMSMESSAGECOPIER_H_
#define UTILS_FACTORIES_MMS_MMSMESSAGECOPIER_H_

#include "../../../message/mms/MmsMessage_m.h"
#include "inet/applications/tcpapp/TcpGenericServerApp.h"

namespace inet {

class MmsMessageCopier {

public:
	MmsMessageCopier();
	virtual ~MmsMessageCopier();

	const Ptr<MmsMessage> copyMessage(const MmsMessage* appmsg, int connId, int serverConnId, bool fakeCreatTime);
	const Ptr<MmsMessage> copyMessage(const MmsMessage* appmsg, int connId, bool fakeCreatTime);
	const Ptr<MmsMessage> copyMessage(const MmsMessage* appmsg, bool fakeCreatTime);
	MmsMessage* copyMessageNorm(const MmsMessage* appmsg, int connId, int evilServerConnId, bool fakeCreatTime);
	MmsMessage* copyMessageNorm(const MmsMessage* appmsg, int connId, bool fakeCreatTime);
	MmsMessage* copyMessageNorm(const MmsMessage* appmsg, bool fakeCreatTime);
};

};

#endif /* UTILS_FACTORIES_MMS_MMSMESSAGECOPIER_H_ */
