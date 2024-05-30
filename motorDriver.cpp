/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */

#include "include/motorDriver.hpp"

MotorDriver::MotorDriver(devicesConfig::Motor* m)
{
	this->mot = m;
	this->power = 0;

	const uint32_t actualFreq = pwm_init(PWM_DEV(this->mot->dev), PWM_LEFT, this->mot->frequency, 1000);

	if(actualFreq != this->mot->frequency)
	{
	    printf("MotorDriver: Error while pwm initializing.\r\n\r\n");
	    if(actualFreq != 0)
	    {
	    	printf("MotorDriver: using frequency: %lu\n", actualFreq);
	    }
	}

	this->stop();
}


void MotorDriver::forward()
{
	pwm_set(PWM_DEV(this->mot->dev), this->mot->reverseChannel, 0);
	pwm_set(PWM_DEV(this->mot->dev), this->mot->forwardChannel, this->power);
}


void MotorDriver::reverse()
{
	pwm_set(PWM_DEV(this->mot->dev), this->mot->reverseChannel, this->power);
	pwm_set(PWM_DEV(this->mot->dev), this->mot->forwardChannel, 0);
}


void MotorDriver::stop()
{
	pwm_set(PWM_DEV(this->mot->dev), this->mot->reverseChannel, 0);
	pwm_set(PWM_DEV(this->mot->dev), this->mot->forwardChannel, 0);
}

void MotorDriver::setPower(uint16_t power)
{
	if(power > 1000) power = 1000;

	this->power = power;
}
