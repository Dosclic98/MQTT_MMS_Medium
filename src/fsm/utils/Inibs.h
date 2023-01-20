/*
 * Inibs.h
 *
 *  Created on: Jan 19, 2023
 *      Author: dosclic98
 */

#ifndef FSM_UTILS_INIBS_H_
#define FSM_UTILS_INIBS_H_

namespace inet {

class Inibs {
public:
	Inibs();
	Inibs(float measureBlockInib, float measureCompromisedInib, float readResponseBlockInib,
			float readResponseCompromisedInib, float commandResponseBlockInib, float commandResponseCompromisedInib);
	virtual ~Inibs();

	inline float getMeasureBlockInib() { return measureBlockInib; }
	inline float getMeasureCompromisedInib() { return measureCompromisedInib; }
	inline float getReadResponseBlockInib() { return readResponseBlockInib; }
	inline float getReadResponseCompromisedInib() { return readResponseCompromisedInib; }
	inline float getCommandResponseBlockInib() { return commandResponseBlockInib; }
	inline float getCommandResponseCompromisedInib() { return commandResponseCompromisedInib; }

private:
	float measureBlockInib;
	float measureCompromisedInib;
	float readResponseBlockInib;
	float readResponseCompromisedInib;
	float commandResponseBlockInib;
	float commandResponseCompromisedInib;



};

};

#endif /* FSM_UTILS_INIBS_H_ */
