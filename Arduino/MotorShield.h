// MotorShield.h

#ifndef _MOTORSHIELD_h
#define _MOTORSHIELD_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

extern const int SPEED_1;
extern const int DIR_1;
extern const int SPEED_2;
extern const int DIR_2;

extern  bool FRWRD;
extern  bool BCKWRD;

void initMotorShield(void);

void motorsGo(bool, bool, int, int);

#endif

