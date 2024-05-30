/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */

#ifndef INCLUDE_WALLFOLLOWER_HPP_
#define INCLUDE_WALLFOLLOWER_HPP_

#include "dualMotorController.hpp"
#include "ultrasonicSensorDriver.hpp"

class WallFollower
{
public:
	WallFollower(DualMotorController* dmc, UltrasonicSensorDriver* sensorM, UltrasonicSensorDriver* sensorR);
	void start();
	void stop();
	~WallFollower();

private:
	DualMotorController* dmc;
	UltrasonicSensorDriver* sensorM;
	UltrasonicSensorDriver* sensorR;
	bool running;
	char* followingThreadStack;
	friend void* wallFollowThread(void* wallFollowerInstance);
};


#endif /* INCLUDE_WALLFOLLOWER_HPP_ */
