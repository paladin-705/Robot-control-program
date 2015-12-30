#pragma once

const int MESSAGE_HEADER_PROPORTIONAL = 42;
const int MESSAGE_HEADER_STANDART = 43;
const int MESSAGE_HEADER_TURNING = 44;
const int MESSAGE_END = 0;

const int SPEED_MODE_PROPORTIONAL = 0;
const int SPEED_MODE_STANDART = 1;
const int SPEED_MODE_TURNING = 2;

const int RECORDING_DEFAULT = 0;
const int RECORDING_START = 1;
const int RECORDING_STOP = 2;

const int LEGO_MODE = 0;
const int ARDUINO_MODE = 1;

//Default setting:
const int DEFAULT_SLEEP_MSEC = 100;

const int DEFAULT_MAX_SPEED = 255;
const int DEFAULT_ARDUINO_SPEED_TRIG = 255;

const int DEFAULT_GAMEPAD_MAXPOS = 32640;
const int DEFAULT_MAX_TRIG_POS_Y = 500;
const int DEFAULT_MIN_TRIG_POS_X = 3000;

const int DEFAULT_MAX_ANGLE = 90;

const int DEFAULT_BCKWRD = false;
const int DEFAULT_FRWRD = true;