/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */

#ifndef DEVICESCONFIG_HPP_
#define DEVICESCONFIG_HPP_

#include "periph/gpio.h"
#include "periph/uart.h"

namespace devicesConfig
{
	struct Motor
	{
		uint32_t forwardChannel;
		uint32_t forwardDev;
		uint32_t reverseChannel;
		uint32_t dev;
		uint32_t frequency;
	};

	struct Pulser
	{
		gpio_t pin;
		bool pullup;
	};

	struct UltrasonicSensor
	{
		gpio_t trigPin;
		gpio_t echoPin;
	};

	struct BluetoothModule
	{
		uart_t uart;
		uint32_t baudrate;
		uart_rx_cb_t rx_callback;
	};

	extern Motor motorRight;
	extern Motor motorLeft;
	extern Pulser pulserRight;
	extern Pulser pulserLeft;
	extern UltrasonicSensor ultrasonicSensorRight;
	extern UltrasonicSensor ultrasonicSensorMiddle;
	extern UltrasonicSensor ultrasonicSensorLeft;
	extern BluetoothModule bluetoothTransmitter;
};



#endif /* DEVICESCONFIG_HPP_ */
