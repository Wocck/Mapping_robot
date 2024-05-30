/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */

#ifndef INCLUDE_POSITIONTRACKER_HPP_
#define INCLUDE_POSITIONTRACKER_HPP_

#include <stdio.h>
#include "pulserDriver.hpp"

class PositionTracker
{
public:
	PositionTracker(PulserDriver* pulserL, PulserDriver* pulserR);
	void start();
	void stop();
	~PositionTracker();

private:
	void update();
	PulserDriver* pulserL;
	PulserDriver* pulserR;
	long previous_left_count;
	long previous_right_count;
	long position_x;
	long position_y;
	double position_theta;

	char* updatingThreadStack;
	bool running;
	friend void* PositionTracker_updatingThread(void*);
};



#endif /* INCLUDE_POSITIONTRACKER_HPP_ */
