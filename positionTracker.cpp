/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */

#include "include/positionTracker.hpp"
#include <stdio.h>
#include <xtimer.h>
#include <math.h>

// physical parameters of the robot
const static long wheelbase = 135L; // distance between the two wheels.
const static long wheel_circumference = 210L;
const static long encoder_resolution = 40L; // impulses per full revolution of wheel
const static int delay = 100;

void* PositionTracker_updatingThread(void* positionTrackerInstance)
{
	printf("ok\r\n");
	PositionTracker* instance = ((PositionTracker*) positionTrackerInstance);

	while(instance->running)
	{
		instance->update();
		printf("%ld,%ld,200\r\n", instance->position_x, instance->position_y);
		xtimer_msleep(delay);
	}

	return nullptr;
}

PositionTracker::PositionTracker(PulserDriver* pulserL, PulserDriver* pulserR)
{
	this->pulserL = pulserL;
	this->pulserR = pulserR;
	this->running = false;
	this->previous_left_count = 0;
	this->previous_right_count = 0;
	this->position_x = 0;
	this->position_y = 0;
	this->position_theta = 0;
	this->updatingThreadStack = new char[THREAD_STACKSIZE_MAIN];
}

void PositionTracker::start()
{
	this->running = true;
	thread_create(this->updatingThreadStack, THREAD_STACKSIZE_MAIN,
					  THREAD_PRIORITY_MAIN - 2, THREAD_CREATE_STACKTEST,
					  PositionTracker_updatingThread, this, "positionTrackerThread");
}


void PositionTracker::stop()
{
	this->running = false;
}


void PositionTracker::update()
{
	long delta_left_count = (long) this->pulserL->read() - this->previous_left_count;
	long delta_right_count = (long) this->pulserR->read() - this->previous_right_count;

	this->previous_left_count = (long) this->pulserL->read();
	this->previous_right_count = (long) this->pulserR->read();

	// Calculate the distance traveled by each wheel:
	long distance_left_wheel = (double) (delta_left_count * wheel_circumference) / encoder_resolution;
	long distance_right_wheel = (double) (delta_right_count * wheel_circumference) / encoder_resolution;

	// Calculate the change in heading of the robot:
	double delta_theta = ((double) (distance_right_wheel - distance_left_wheel)) / wheelbase;

	// Calculate the average distance traveled by the robot:
	long distance_average = (distance_left_wheel + distance_right_wheel) / 2;

	// Update the robot's position and orientation:
	this->position_x += distance_average * cos(this->position_theta + (delta_theta / 2));
	this->position_y += distance_average * sin(this->position_theta + (delta_theta / 2));
	this->position_theta += delta_theta;
}

PositionTracker::~PositionTracker()
{
	delete [] this->updatingThreadStack;
}
