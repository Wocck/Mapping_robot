/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */

#include "include/wallFollower.hpp"
#include <thread.h>
#include <xtimer.h>

#define P_GAIN 1.1
#define D_GAIN 0.3

void* wallFollowThread(void* wallFollowerInstance)
{
	WallFollower* follower = (WallFollower*) wallFollowerInstance;

	follower->dmc->forward(500, 0);
	while(follower->sensorM->measure() > 250)
	{
		xtimer_msleep(100);
		if(!follower->running)
		{
			follower->dmc->stop();
			return nullptr;
		}
	}

	follower->dmc->rotateL(80, 500);

	long prevErrorSignal = 0;
	long direction = 0;

	while(follower->running)
	{
		xtimer_msleep(10);
		if(follower->sensorM->measure() < 250)
		{
			follower->dmc->rotateL(80, 400);
		}

		long errorSignal = follower->sensorR->measure() - 250;

		direction = (errorSignal * P_GAIN) + ((errorSignal - prevErrorSignal) * D_GAIN);

		if(direction > 250) direction = 250;
		if(direction < -250) direction = -250;

		follower->dmc->forward(400, direction);
	}

	follower->dmc->stop();
	return nullptr;
}

WallFollower::WallFollower(DualMotorController* dmc, UltrasonicSensorDriver* sensorM, UltrasonicSensorDriver* sensorR)
{
	this->dmc = dmc;
	this->sensorR = sensorR;
	this->sensorM = sensorM;
	this->followingThreadStack = new char[THREAD_STACKSIZE_MAIN];
	this->running = false;
}

void WallFollower::start()
{
	this->running = true;
	thread_create(this->followingThreadStack, THREAD_STACKSIZE_MAIN,
				  THREAD_PRIORITY_MAIN - 2, THREAD_CREATE_STACKTEST,
				  wallFollowThread, this, "wallFollowingThread");
}

void WallFollower::stop()
{
	this->running = false;
}

WallFollower::~WallFollower()
{
	delete [] this->followingThreadStack;
}
