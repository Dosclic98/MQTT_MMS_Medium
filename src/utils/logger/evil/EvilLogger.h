/*
 * EvilLogger.h
 *
 *  Created on: Jan 23, 2023
 *      Author: dosclic98
 */

#ifndef UTILS_LOGGER_EVIL_EVILLOGGER_H_
#define UTILS_LOGGER_EVIL_EVILLOGGER_H_

#include "../mms/MmsPacketLogger.h"
#include "../../../fsm/states/evil/EvilState.h"

namespace inet {

class EvilLogger : public MmsPacketLogger {
public:
	EvilLogger(int loggerIndex);
	void log(const MmsMessage* msg, EvilStateName evilState, simtime_t timestamp);

	virtual ~EvilLogger();
private:
	int loggerIndex;
};

};

#endif /* UTILS_LOGGER_EVIL_EVILLOGGER_H_ */
