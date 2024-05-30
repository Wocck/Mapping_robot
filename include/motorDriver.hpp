/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */

#ifndef DRIVERS_MOTOR_HPP_
#define DRIVERS_MOTOR_HPP_

#include "periph/gpio.h"
#include "periph/pwm.h"
#include <stdio.h>
#include "devicesConfig.hpp"

class MotorDriver
{
public:
	MotorDriver(devicesConfig::Motor* m);
	void forward();
	void reverse();
	void stop();
	void setPower(uint16_t power); // power from 0 to 1000

private:
	devicesConfig::Motor* mot;
	uint16_t power;
};


#endif /* DRIVERS_MOTOR_HPP_ */
