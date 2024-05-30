/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */
#include "include/devicesConfig.hpp"

devicesConfig::Motor devicesConfig::motorRight =
{
	.forwardChannel = 2,
	.reverseChannel = 1,
	.dev = 0,
	.frequency = 5000
};

devicesConfig::Motor devicesConfig::motorLeft =
{
	.forwardChannel = 0,
	.reverseChannel = 2,
	.dev = 2,
	.frequency = 5000
};

devicesConfig::Pulser devicesConfig::pulserRight =
{
	.pin = GPIO_PIN(PORT_B, 2),
	.pullup = true
};

devicesConfig::Pulser devicesConfig::pulserLeft =
{
	.pin = GPIO_PIN(PORT_B, 0),
	.pullup = true
};

devicesConfig::UltrasonicSensor devicesConfig::ultrasonicSensorRight =
{
	.trigPin = GPIO_PIN(PORT_C, 3),
	.echoPin = GPIO_PIN(PORT_C, 2)
};

devicesConfig::UltrasonicSensor devicesConfig::ultrasonicSensorMiddle =
{
	.trigPin = GPIO_PIN(PORT_C, 12),
	.echoPin = GPIO_PIN(PORT_C, 10)
};

devicesConfig::UltrasonicSensor devicesConfig::ultrasonicSensorLeft =
{
	.trigPin = GPIO_PIN(PORT_A, 4),
	.echoPin = GPIO_PIN(PORT_A, 5)
};

devicesConfig::BluetoothModule devicesConfig::bluetoothTransmitter =
{
	.uart = 2,
	.baudrate = 9600,
	.rx_callback = nullptr
};

