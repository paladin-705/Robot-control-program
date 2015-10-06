// 
// 
// 

#include "MotorShield.h"
#include <Arduino.h>

//���� ����������:
const int SPEED_1 = 5;
const int DIR_1 = 4;
const int SPEED_2 = 6;
const int DIR_2 = 7;

/*��������� ��� ����������� �������� �������
(� ������ ���� ������ ����� ��������� �� � �� ������� ���� ������� ��������
�������� �� ���������������)*/
bool FRWRD = true;
bool BCKWRD = false;

void initMotorShield()
{
	for (int i = 4; i <= 7; i++)
		pinMode(i, OUTPUT);
}

void motorsGo(bool motorDirection_1, bool motorDirection_2, int speed_1, int speed_2)
{
	if (speed_1 < 0) speed_1 = 0;
	if (speed_2 < 0) speed_2 = 0;

	if (speed_1 > 255) speed_1 = 255;
	if (speed_2 > 255) speed_2 = 255;

	analogWrite(SPEED_1, speed_1);
	analogWrite(SPEED_2, speed_2);

	digitalWrite(DIR_1, motorDirection_1);
	digitalWrite(DIR_2, motorDirection_2);
}
