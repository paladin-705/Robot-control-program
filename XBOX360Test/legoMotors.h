#pragma once

//Lego ev3sources https://github.com/mindboards/ev3sources
#define   PRIMPAR_SHORT                 0x00
#define   PRIMPAR_LONG                  0x80

#define   PRIMPAR_CONST                 0x00

#define   PRIMPAR_VALUE                 0x3F

#define   PRIMPAR_1_BYTE                1

#define   DIRECT_COMMAND_NO_REPLY       0x80    //  Direct command, reply not required

#define	  opOUTPUT_SPEED		0xA5	//  10100101
#define	  opOUTPUT_POWER		0xA4	//  10100100
#define	  opOUTPUT_POSITION		0xAB	//  10101011
#define   opOUTPUT_START		0xA6	//  10100110

#define   LC0(v)                        ((v & PRIMPAR_VALUE) | PRIMPAR_SHORT | PRIMPAR_CONST)
#define   LC1(v)                        (PRIMPAR_LONG  | PRIMPAR_CONST | PRIMPAR_1_BYTE),(v & 0xFF)
//----------------------------

enum MotorNumb
{
	MotorA = 1,
	MotorB = 2,
	MotorC = 4,
	MotorD = 8,
};

enum Role
{
	Left = 0,
	Right = 1,
	Turning = 2
};

class LegoMotors
{
public:
	LegoMotors();
	~LegoMotors();

	inline int getRole() { return _role; };
	inline void setRole(Role role) { _role = role; };

	inline int getMotorNumb() { return _motorNumber; };
	inline void setMotorNumb(MotorNumb numb) { _motorNumber = numb; };

	inline int getSpeed() { return _speed; };
	inline void setSpeed(int speed) { _speed = speed; };

	inline int getAngle() { return _angle; };
	inline void setAngle(int angle) { _angle = angle; };

	inline bool getInvertState() { return _inversion; };
	inline void setInvertState(bool inversion) { _inversion = inversion; };

	inline bool getMotorState() { return _motorState; };
	inline void setMotorState(bool motorState) { _motorState = motorState; };

private:
	MotorNumb _motorNumber;	
	Role _role;			//Left, Right, Turning
	bool _motorState;	//active or passive
	bool _inversion;
	int _speed;
	int _angle;
};

