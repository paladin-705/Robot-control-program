#pragma once

#include "CXBOXController.h"
#include "Serial lib\Serial.h"
#include "legoMotors.h"
#include "InputFunctions.h"
#include "SettingFunc.h"

#include "Constants.h"
#include "GlobalVarSettings.h"

//------------------------------
#include <iostream>
#include <iomanip>

#include <mutex>

#include <fstream>
#include <ctime>
#include <vector>

//Motor controll:
void motorsControllArduino(int, int, unsigned char *, unsigned char *, unsigned char *, unsigned char *);
void motorsControllLego(LegoMotors *, int, int, int);

//Send Message Thread:
void sendMessageThread(CXBOXController *, Serial *, int, int, int);
void stopSendMessageThread();
