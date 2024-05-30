/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */

#include "include/pulserDriver.hpp"
#include <stdio.h>
#include <thread.h>
#include <xtimer.h>

void* PulserDriver_countingThread(void* driverInstance)
{
	int previousState = gpio_read(((PulserDriver*) driverInstance)->pulser->pin);
	while(true)
	{
		xtimer_usleep(100);
		int actualState = gpio_read(((PulserDriver*) driverInstance)->pulser->pin);
		if(actualState != previousState)
		{
			previousState = actualState;
			((PulserDriver*) driverInstance)->pulses.do_not_access_from_cpp++;
		}
	}
	return nullptr;
}

PulserDriver::PulserDriver(devicesConfig::Pulser* p)
{
	this->pulser = p;
	this->pulses.do_not_access_from_cpp = 0;


	if(gpio_init(this->pulser->pin, (this->pulser->pullup) ? GPIO_IN_PU : GPIO_IN))
	{
		printf("PulserDriver: Error while pin initializing\r\n\r\n");
	}

	this->countingThreadStack = new char[THREAD_STACKSIZE_MAIN];

	thread_create(this->countingThreadStack, THREAD_STACKSIZE_MAIN,
	                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
					  PulserDriver_countingThread, this, "countingThread");
}

unsigned long PulserDriver::read()
{
	return this->pulses.do_not_access_from_cpp;
}

void PulserDriver::clear()
{
	this->pulses.do_not_access_from_cpp = 0;
}

