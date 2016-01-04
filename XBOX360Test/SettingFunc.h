#pragma once


#include "Constants.h"
#include "InputFunctions.h"
#include "GlobalVarSettings.h"
#include "legoMotors.h"

//------------------------------
#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <iterator>

//Default setting:
const int DEFAULT_SLEEP_MSEC = 100;

const int DEFAULT_MAX_SPEED_ARDUINO = 255;
const int DEFAULT_ARDUINO_SPEED_TRIG = 255;

const int DEFAULT_MAX_SPEED_LEGO = 100;
const int DEFAULT_LEGO_SPEED_TRIG = 100;

const int DEFAULT_GAMEPAD_MAXPOS = 32640;
const int DEFAULT_MAX_TRIG_POS_Y = 500;
const int DEFAULT_MIN_TRIG_POS_X = 3000;

const int DEFAULT_MAX_ANGLE = 90;

const int DEFAULT_BCKWRD = false;
const int DEFAULT_FRWRD = true;

//Setting:
void changeSetting();
void defaultSetting();
void loadLegoSetting(LegoMotors *, int);
void loadSetting();
