// main.cpp: 
//
#include "Serial lib\Serial.h"
#include "CXBOXController.h"
#include "SendMessageThread.h"

#include "InputFunctions.h"
#include "SettingFunc.h"

#include "GlobalVarSettings.h"
#include "Constants.h"

#include <iostream>	
#include <iomanip>	
#include <string>	
#include <thread>	
#include <mutex>	

using namespace std;

//Send Message Thread:
extern bool flgStop;		//Exit on sendMessageThread
extern int flgRecording;
extern string replayPatch;
extern mutex g_lock;
//--------------------------


//Gamepad setting:
void enterGamepadMode(int *, int *);

int main()
{
	Serial serial;
	CXBOXController* Player;
	int progMode;

	int legoArduinoMode;
	int nmbGmpd;
	int sMode;
	int wMode;
	wchar_t buffPortName[7];

	coutMessage(Header, "Program mode: ");
	cout << "0-Lego Mode" << endl;
	cout << "1-Arduino Mode" << endl;
	cout << "2-Setting" << endl;
	coutMessage(EnterData, "Enter Program mode: ");
	cin >> progMode;

	switch (progMode)
	{
	case 0:
	{
		legoArduinoMode = LEGO_MODE;
		break;
	}
	case 1:
	{
		legoArduinoMode = ARDUINO_MODE;
		break;
	}
	case 2:
	{
		changeSetting();
		return 0;
		break;
	}
	default:
		return 0;
		break;
	}
	system("cls");

	defaultSetting();
	loadSetting();

	coutMessage(Header, "Work mode: ");
	cout << "0-Control" << endl;
	cout << "1-Record patch" << endl;
	cout << "2-Play patch" << endl;

	coutMessage(Line);
	coutMessage(EnterData, "Enter Work mode: ");
	cin >> wMode;
	coutMessage(Line);

error:
	flgStop = false;
	flgRecording = RECORDING_DEFAULT;

	coutMessage(EnterData, "Enter COM port number(COM_): ");
	wcin >> buffPortName;

	//---------------------------
	serial.setCOMname(buffPortName);

	switch (serial.initCOM())
	{
	case 0:
	{
		coutMessage(Success, "COM port conected");
		delayAndCls();
		break;
	}
	case 1:
	{
		coutMessage(Error, "Serial port does not exist.");
		delayAndCls();

		return 0;
		break;
	}
	case 2:
	{
		coutMessage(Error, "Some other error occurred");
		delayAndCls();

		return 0;
		break;
	}
	case 3:
	{
		coutMessage(Error, "Getting state error");
		delayAndCls();

		return 0;
		break;
	}
	case 4:
	{
		coutMessage(Error, "Error setting serial port state");
		delayAndCls();

		return 0;
		break;
	}
	}
	//---------------------------

	switch (wMode)
	{
	case 0:
	{
		enterGamepadMode(&nmbGmpd, &sMode);

		Player = new CXBOXController(nmbGmpd);
		//---------------------------

		thread sendMesThr(sendMessageThread, Player, &serial, sMode, wMode, legoArduinoMode);	//????????? ????? ??? ???????? ?????? ?? Arduino

		system("cls");

		if (Player->IsConnected())
		{
			while (true)
			{
				if (Player->IsConnected())
				{
					cls();

					cout << "Work mode: " << wMode << "   " << "Serial port: ";
					wcout << buffPortName << endl;

					coutMessage(GamepadData, "Left stick:");
					cout << "X pos: " << setw(6) << Player->GetState().Gamepad.sThumbLX << " " << "Y pos: " << setw(6) << Player->GetState().Gamepad.sThumbLY << endl;
					coutMessage(GamepadData, "Right stick:");
					cout << "X pos: " << setw(6) << Player->GetState().Gamepad.sThumbRX << " " << "Y pos: " << setw(6) << Player->GetState().Gamepad.sThumbRY << endl;
					SetColor(LightGray);

					if (Player->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
					{
						stopSendMessageThread();
						system("pause");
						break;
					}
				}
				else
				{
					coutMessage(Error, "ERROR! XBOX 360 Controller Not Found!");

					stopSendMessageThread();

					system("pause");
					break;
				}
			}
		}
		else
		{
			coutMessage(Error, "ERROR! XBOX 360 Controller Not Found!");

			stopSendMessageThread();

			delete(Player);
			serial.close();

			delayAndCls();
		}

		delete(Player);
		sendMesThr.detach();
		break;
	}
	case 1:
	{
		enterGamepadMode(&nmbGmpd, &sMode);

		Player = new CXBOXController(nmbGmpd);
		//---------------------------
		thread sendMesThr(sendMessageThread, Player, &serial, sMode, wMode, legoArduinoMode);
	
		system("cls");

		if (Player->IsConnected())
		{
			int recordingState = 0;

			while (true)
			{
				if (Player->IsConnected())
				{
					cls();
					cout << "Work mode: " << wMode << "   " << "Serial port: ";
					wcout << buffPortName << endl;

					if ((Player->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) && (recordingState == 0))
					{
						g_lock.lock();
						flgRecording = RECORDING_START;
						recordingState = 1;
						g_lock.unlock();
					}

					if ((Player->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) && (recordingState == 1))
					{
						g_lock.lock();
						flgRecording = RECORDING_STOP;
						recordingState = 2;
						g_lock.unlock();
					}
					
					if (recordingState == 1)
					{
						coutMessage(RecordingStart, "Recording enable");
					}
					if (recordingState == 2)
					{
						coutMessage(RecordingStop, "Recording disable");
					}

					coutMessage(GamepadData, "Left stick:");
					cout << "X pos: " << setw(6) << Player->GetState().Gamepad.sThumbLX << " " << "Y pos: " << setw(6) << Player->GetState().Gamepad.sThumbLY << endl;
					coutMessage(GamepadData, "Right stick:");
					cout << "X pos: " << setw(6) << Player->GetState().Gamepad.sThumbRX << " " << "Y pos: " << setw(6) << Player->GetState().Gamepad.sThumbRY << endl;
					SetColor(LightGray);

					if (Player->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
					{
						stopSendMessageThread();
						system("pause");
						break;
					}
				}
				else
				{
					coutMessage(Error, "ERROR! XBOX 360 Controller Not Found!");

					stopSendMessageThread();

					system("pause");
					break;
				}
			}
		}
		else
		{
			coutMessage(Error, "ERROR! XBOX 360 Controller Not Found!");

			stopSendMessageThread();

			delete(Player);
			serial.close();

			delayAndCls();
		}

		delete(Player);
		sendMesThr.detach();
		break;
	}
	case 2:
	{
		coutMessage(EnterData, "Enter file patch: ");
		cin >> replayPatch;
		SetColor(LightGray);
		system("cls");

		thread sendMesThr(sendMessageThread, Player, &serial, sMode, wMode, legoArduinoMode);
		sendMesThr.join();
		delayAndCls();
		break;
	}
	}

	stopSendMessageThread();
	serial.close();

	return 0;
}

//Gamepad setting:
void enterGamepadMode(int *nmbGmpd, int *sMode)
{
	bool flg;
	do {
		coutMessage(EnterData, "Enter number of gamepad(1-4): ");
		cin >> *nmbGmpd;

		if (*nmbGmpd < 1 || *nmbGmpd > 4)
		{
			coutMessage(Error, "incorrect number");
			delayAndCls();
			flg = true;
		}
		else flg = false;

	} while (flg);

	coutMessage(Line);

	coutMessage(Header, "Control mode: ");

	std::cout << "0-Left stick" << std::endl;
	std::cout << "1-Left(Y axis) and Right(X axis) stick)" << std::endl;
	std::cout << "2-Left(X axis) and Right(Y axis) stick)" << std::endl;
	std::cout << "3-Right stick" << std::endl;
	
	coutMessage(Line);
	
	coutMessage(EnterData, "Enter Stick mode: ");
	std::cin >> *sMode;

	SetColor(LightGray);
}
