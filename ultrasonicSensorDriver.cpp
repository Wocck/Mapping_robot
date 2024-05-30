/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */

#include "include/ultrasonicSensorDriver.hpp"
#include <stdio.h>
#include <xtimer.h>

UltrasonicSensorDriver::UltrasonicSensorDriver(devicesConfig::UltrasonicSensor* s)
{
	this->sensor = s;

	if(gpio_init(this->sensor->trigPin, GPIO_OUT))
	{
		printf("UltrasonicSensorDriver: Error while trigger pin initializing\r\n\r\n");
	}

	if(gpio_init(this->sensor->echoPin, GPIO_IN))
	{
		printf("UltrasonicSensorDriver: Error while echo pin initializing\r\n\r\n");
	}

	gpio_clear(this->sensor->trigPin);
}

unsigned long UltrasonicSensorDriver::measure()
{
	gpio_set(this->sensor->trigPin);
	xtimer_usleep(10);
	gpio_clear(this->sensor->trigPin);

	unsigned long time = xtimer_now();

	while(gpio_read(this->sensor->echoPin) == 0)
	{
		if((xtimer_now() - time) > 100000)
		{
			printf("UltrasonicSensorDriver: Error while doing measurement. Echo signal not received (timeout).\r\n\r\n");
			return 0;
		}
	}

	time = xtimer_now();

	while(gpio_read(this->sensor->echoPin))
	{
		if((xtimer_now() - time) > 100000)
		{
			printf("UltrasonicSensorDriver: Error while doing measurement. Echo signal too long (timeout).\r\n\r\n");
			return 0;
		}
	}

	return ((xtimer_now() - time) * 10) / 58; // Result in [mm]
}
