/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */

#ifndef INCLUDE_ULTRASONICSENSORDRIVER_HPP_
#define INCLUDE_ULTRASONICSENSORDRIVER_HPP_

#include "devicesConfig.hpp"

class UltrasonicSensorDriver
{
public:
	UltrasonicSensorDriver(devicesConfig::UltrasonicSensor* s);
	unsigned long measure();

private:
	devicesConfig::UltrasonicSensor* sensor;
};


#endif /* INCLUDE_ULTRASONICSENSORDRIVER_HPP_ */
