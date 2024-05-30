/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */

#ifndef INCLUDE_DUALMOTORCONTROLLER_HPP_
#define INCLUDE_DUALMOTORCONTROLLER_HPP_

#include "motorDriver.hpp"
#include "pulserDriver.hpp"

class DualMotorController
{
public:
	DualMotorController(MotorDriver* motorR, MotorDriver* motorL, PulserDriver* pulserL, PulserDriver* pulserR, int16_t bias);
	void forward(uint16_t power, int16_t direction); // power from 0 to 1000 nad direction from 0 to 1000
	void stop();
	void rotateL(uint16_t degrees, uint16_t power);

private:
	MotorDriver* motorL;
	MotorDriver* motorR;
	PulserDriver* pulserL;
	PulserDriver* pulserR;
	int16_t bias;
};



#endif /* INCLUDE_DUALMOTORCONTROLLER_HPP_ */
