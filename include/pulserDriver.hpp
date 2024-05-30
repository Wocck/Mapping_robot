/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */

#ifndef INCLUDE_PULSERDRIVER_HPP_
#define INCLUDE_PULSERDRIVER_HPP_

#include "devicesConfig.hpp"
#include "c11_atomics_compat.hpp"


class PulserDriver
{
public:
	PulserDriver(devicesConfig::Pulser* p);
	unsigned long read();
	void clear();

private:
	char* countingThreadStack;
	atomic_ulong pulses;
	devicesConfig::Pulser* pulser;
	friend void* PulserDriver_countingThread(void*);
};


#endif /* INCLUDE_PULSERDRIVER_HPP_ */
