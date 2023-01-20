/*
 * Inibs.cc
 *
 *  Created on: Jan 19, 2023
 *      Author: dosclic98
 */

#include "Inibs.h"
#include <stdexcept>

namespace inet {

Inibs::Inibs()
{
	// TODO Auto-generated constructor stub
}

Inibs::Inibs(float measureBlockInib, float measureCompromisedInib, float readRequestBlockInib, float readRequestCompromisedInib,
		float commandRequestBlockInib, float commandRequestCompromisedInib, float readResponseBlockInib,
		float readResponseCompromisedInib, float commandResponseBlockInib, float commandResponseCompromisedInib) {

	if(measureBlockInib < 0 || measureBlockInib > 1 || measureCompromisedInib < 0 || measureCompromisedInib > 1 ||
			readRequestBlockInib < 0 || readRequestBlockInib > 1 || readRequestCompromisedInib < 0 || readRequestCompromisedInib > 1 ||
			commandRequestBlockInib < 0 || commandRequestBlockInib > 1 || commandRequestCompromisedInib < 0 || commandRequestCompromisedInib > 1 ||
			readResponseBlockInib < 0 ||  readResponseBlockInib > 1 || readResponseCompromisedInib < 0 || readResponseCompromisedInib > 1  ||
			commandResponseBlockInib < 0 || commandResponseBlockInib > 1 || commandResponseCompromisedInib < 0 || commandResponseCompromisedInib > 1) {

		throw std::invalid_argument("Invalid inhibition value must be specified between 0 and 1");
	}
	this->measureBlockInib = measureBlockInib;
	this->measureCompromisedInib = measureCompromisedInib;
	this->readRequestBlockInib = readRequestBlockInib;
	this->readRequestCompromisedInib = readRequestCompromisedInib;
	this->commandRequestBlockInib = commandRequestBlockInib;
	this->commandRequestCompromisedInib = commandRequestCompromisedInib;
	this->readResponseBlockInib = readResponseBlockInib;
	this->readResponseCompromisedInib = readResponseCompromisedInib;
	this->commandResponseBlockInib = commandResponseBlockInib;
	this->commandResponseCompromisedInib = commandResponseCompromisedInib;
}

Inibs::~Inibs()
{
	// TODO Auto-generated destructor stub
}

};

