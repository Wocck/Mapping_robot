/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */
#include "include/dualMotorController.hpp"
#include "xtimer.h"

DualMotorController::DualMotorController(MotorDriver* motorR, MotorDriver* motorL, PulserDriver* pulserL, PulserDriver* pulserR, int16_t bias)
{
	this->motorR = motorR;
	this->motorL = motorL;
	this->pulserR = pulserR;
	this->pulserL = pulserL;
	this->bias = bias;
}

void DualMotorController::forward(uint16_t power, int16_t direction)
{
	direction += bias;
	if(direction < -1000) direction = -1000;
	if(direction > 1000) direction = 1000;

	int32_t difference = direction;
	difference *= power;
	difference /= 1000;

	this->motorL->setPower(power + difference);
	this->motorR->setPower(power - difference);

	this->motorL->forward();
	this->motorR->forward();
}

void DualMotorController::stop()
{
	this->motorL->stop();
	this->motorR->stop();
}

void DualMotorController::rotateL(uint16_t degrees, uint16_t power)
{
	this->motorL->stop();
	this->motorR->stop();

	long unsigned int pulses = 0,
					  pulsesR = this->pulserR->read();

	pulses = ((((double) degrees) / 360.0) * 200.0) - 7;

	this->motorL->setPower(power);
	this->motorR->setPower(power);

	this->motorR->forward();

	while((pulsesR + pulses) > this->pulserR->read()) { xtimer_usleep(100); }

	this->motorR->stop();
}
