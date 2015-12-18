// main.cpp: 
//
#include "Serial lib\Serial.h"
#include "CXBOXController.h"
#include "Functions.h"

#include <iostream>
#include <iomanip> 
#include <Windows.h>

#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <iterator>

#include <thread>
#include <mutex>

using namespace std;

mutex g_lock;

Serial serial;
CXBOXController* Player;

//sendMessageThread const:
bool flgStop;

#define RECORDING_DEFAULT 0
#define RECORDING_START 1
#define RECORDING_STOP 2
int flgRecording;

const int MESSAGE_HEADER = 42;
const int MESSAGE_END = 0;

string replayPatch;

int SLEEP_MSEC = 100;
//motorsControll func const:
bool SPEED_MODE = true;

int MAX_TRIG_POS_Y = 500;
int MIN_TRIG_POS_X = 3000;

int SPEED_TRIG = 255;

bool BCKWRD = false;
bool FRWRD = true;

void loadSetting();
void enterGamepadMode(int *, int *);
void motorsControll(int, int, unsigned char *, unsigned char *, unsigned char *, unsigned char *);
void sendMessageThread(int, int);

int main()
{
	SetColor(LightGray);
	loadSetting();

	int nmbGmpd;
	int sMode;
	int wMode;
	wchar_t buffPortName[7];

	SetColor(White);
	cout << "Work mode: " << endl;
	SetColor(LightGray);
	cout << "0-Control" << endl;
	cout << "1-Record patch" << endl;
	cout << "2-Play patch" << endl;

	SetColor(Cyan);
	cout << "----------------------------------" << endl;
	SetColor(White);
	cout << "Enter Work mode: ";
	SetColor(Yellow);
	cin >> wMode;

	SetColor(Cyan);
	cout << "----------------------------------" << endl;

error:
	flgStop = false;
	flgRecording = RECORDING_DEFAULT;

	SetColor(White);
	cout << "Enter COM port number(COM_): ";
	SetColor(Yellow);
	wcin >> buffPortName;
	SetColor(Cyan);
	cout << "----------------------------------" << endl;

	//---------------------------
	serial.setCOMname(buffPortName);

	switch (serial.initCOM())
	{
	case 0:
	{
		SetColor(Green);
		std::cout << "COM port conected\n";

		SetColor(Cyan);
		cout << "----------------------------------" << endl;

		SetColor(LightGray);
		delayAndCls();
		break;
	}
	case 1:
	{
		SetColor(Red);
		std::cout << "\aSerial port does not exist.\n";

		SetColor(Cyan);
		cout << "----------------------------------" << endl;

		SetColor(LightGray);
		delayAndCls();

		return 0;
		break;
	}
	case 2:
	{
		SetColor(Red);
		std::cout << "\aSome other error occurred.\n";

		SetColor(Cyan);
		cout << "----------------------------------" << endl;

		SetColor(LightGray);
		delayAndCls();

		return 0;
		break;
	}
	case 3:
	{
		SetColor(Red);
		std::cout << "\aGetting state error\n";

		SetColor(Cyan);
		cout << "----------------------------------" << endl;

		SetColor(LightGray);
		delayAndCls();

		return 0;
		break;
	}
	case 4:
	{
		SetColor(Red);
		std::cout << "\aError setting serial port state\n";

		SetColor(Cyan);
		cout << "----------------------------------" << endl;

		SetColor(LightGray);
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

		thread sendMesThr(sendMessageThread, ref(sMode), ref(wMode));	//????????? ????? ??? ???????? ?????? ?? Arduino

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

					SetColor(Green);
					cout << "Left stick:\n";
					SetColor(White);
					cout << "X pos: " << setw(6) << Player->GetState().Gamepad.sThumbLX << " " << "Y pos: " << setw(6) << Player->GetState().Gamepad.sThumbLY << endl;
					SetColor(Green);
					cout << "Right stick:\n";
					SetColor(White);
					cout << "X pos: " << setw(6) << Player->GetState().Gamepad.sThumbRX << " " << "Y pos: " << setw(6) << Player->GetState().Gamepad.sThumbRY << endl;
					SetColor(LightGray);

					if (Player->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
					{
						g_lock.lock();
						flgStop = true;
						g_lock.unlock();

						system("pause");
						break;
					}
				}
				else
				{
					SetColor(Red);
					cout << "----------------------------------" << endl;
					SetColor(LightRed);
					std::cout << "\aERROR! PLAYER " << nmbGmpd << " - XBOX 360 Controller Not Found!\n";
					std::cout << "Press Any Key To Exit.\n";
					SetColor(Red);
					cout << "----------------------------------" << endl;
					SetColor(LightGray);

					g_lock.lock();
					flgStop = true;
					g_lock.unlock();

					system("pause");
					break;
				}
			}
		}
		else
		{
			SetColor(Red);
			cout << "----------------------------------" << endl;
			SetColor(LightRed);
			std::cout << "\aERROR! PLAYER " << nmbGmpd << " - XBOX 360 Controller Not Found!\n";
			SetColor(Red);
			cout << "----------------------------------" << endl;
			SetColor(LightGray);

			g_lock.lock();
			flgStop = true;
			g_lock.unlock();

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
		thread sendMesThr(sendMessageThread, sMode, wMode);
	
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
						SetColor(Green);
						cout << "Recording enable" << endl;
						SetColor(LightGray);
					}
					if (recordingState == 2)
					{
						SetColor(Red);
						cout << "Recording disable" << endl;
						SetColor(LightGray);
					}

					SetColor(Green);
					cout << "Left stick:\n";
					SetColor(White);
					cout << "X pos: " << setw(6) << Player->GetState().Gamepad.sThumbLX << " " << "Y pos: " << setw(6) << Player->GetState().Gamepad.sThumbLY << endl;
					SetColor(Green);
					cout << "Right stick:\n";
					SetColor(White);
					cout << "X pos: " << setw(6) << Player->GetState().Gamepad.sThumbRX << " " << "Y pos: " << setw(6) << Player->GetState().Gamepad.sThumbRY << endl;
					SetColor(LightGray);

					if (Player->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
					{
						g_lock.lock();
						flgStop = true;
						g_lock.unlock();

						system("pause");
						break;
					}
				}
				else
				{
					SetColor(Red);
					cout << "----------------------------------" << endl;
					SetColor(LightRed);
					std::cout << "\aERROR! PLAYER " << nmbGmpd << " - XBOX 360 Controller Not Found!\n";
					std::cout << "Press Any Key To Exit.\n";
					SetColor(Red);
					cout << "----------------------------------" << endl;
					SetColor(LightGray);

					g_lock.lock();
					flgStop = true;
					g_lock.unlock();

					system("pause");
					break;
				}
			}
		}
		else
		{
			SetColor(Red);
			cout << "----------------------------------" << endl;
			SetColor(LightRed);
			std::cout << "\aERROR! PLAYER " << nmbGmpd << " - XBOX 360 Controller Not Found!\n";
			SetColor(Red);
			cout << "----------------------------------" << endl;
			SetColor(LightGray);

			g_lock.lock();
			flgStop = true;
			g_lock.unlock();

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
		SetColor(White);
		cout << "Enter file patch: ";
		SetColor(Yellow);
		cin >> replayPatch;
		SetColor(LightGray);
		system("cls");

		thread sendMesThr(sendMessageThread, sMode, wMode);
		sendMesThr.join();
		delayAndCls();
		break;
	}
	}

	g_lock.lock();
	flgStop = true;
	g_lock.unlock();
	serial.close();

	return 0;
}

void loadSetting()
{
	ifstream setting("setting.ini");
	string varName;
	int value;

	while (!setting.eof())
	{
		setting >> varName;
		setting >> value;

		if (varName == "SLEEP_MSEC:") SLEEP_MSEC = value;
		if (varName == "MAX_TRIG_POS_Y:") MAX_TRIG_POS_Y = value;
		if (varName == "MIN_TRIG_POS_X:") MIN_TRIG_POS_X = value;
		if (varName == "SPEED_TRIG:") {
			if (value >= 0 || value <= 255) SPEED_TRIG = value;
		}
		if (varName == "BCKWRD(0or1):") {

			if (value == 0 || value == 1) BCKWRD = value;
		}
		if (varName == "FRWRD(0or1):") {
			if (value == 0 || value == 1) FRWRD = value;
		}
		if (varName == "SPEED_MODE(0or1):") SPEED_MODE = value;
	}
	setting.close();
}

void motorsControll(int buffPosX, int buffPosY, unsigned char *directionL, unsigned char *directionR, unsigned char *speedLeft, unsigned char *speedRight)
{

	float MAXPOS = 32640;

	float posX = fabs(buffPosX / MAXPOS);
	float posY = fabs(buffPosY / MAXPOS);

	if (fabs(buffPosX) < 129)posX = 0;
	if (fabs(buffPosY) < 129)posY = 0;


	float coefSpeedDif;

	if (buffPosX<0) {
		if (SPEED_MODE)
		{
			*speedRight = 255 * sqrt(2 * posY - pow(posY, 2));
			coefSpeedDif = sqrt(1 - pow(posX, 2));
			*speedLeft = *speedRight *coefSpeedDif;
		}
		else
		{
			*speedRight = 255 * posY;
			coefSpeedDif = sqrt(1 - pow(posX, 2));
			*speedLeft = *speedRight *coefSpeedDif;
		}
	}
	else {
		if (SPEED_MODE)
		{
			*speedLeft = 255 * sqrt(2 * posY - pow(posY, 2));
			coefSpeedDif = sqrt(1 - pow(posX, 2));
			*speedRight = *speedLeft*coefSpeedDif;
		}
		else
		{
			*speedLeft = 255 * posY;
			coefSpeedDif = sqrt(1 - pow(posX, 2));
			*speedRight = *speedLeft*coefSpeedDif;
		}
	}

	//Trigger 
	if ((fabs(buffPosY) <= MAX_TRIG_POS_Y) && (fabs(buffPosX) >= MIN_TRIG_POS_X)) {
		if (buffPosX<0) {
			*directionL = BCKWRD;
			*directionR = FRWRD;
			*speedLeft = SPEED_TRIG;
			*speedRight = SPEED_TRIG;
		}
		else {
			*directionL = FRWRD;
			*directionR = BCKWRD;
			*speedLeft = SPEED_TRIG;
			*speedRight = SPEED_TRIG;
		}
	}
	else {
		if (buffPosY<0) {
			*directionL = BCKWRD;
			*directionR = BCKWRD;
		}
		else if (buffPosY>0) {
			*directionL = FRWRD;
			*directionR = FRWRD;
		}
	}
}

void sendMessageThread(int stickMode, int workMode)
{
	int posX;
	int posY;

	unsigned char message[6];
	/*
	0 - header
	1 - left motor direction
	2 - right motor direction
	3 - left motor speed
	4 - right motor speed
	5 - end
	*/
	message[0] = MESSAGE_HEADER;
	message[5] = MESSAGE_END;

	switch (workMode)
	{
	case 0:
	{
		do
		{
			g_lock.lock();

			switch (stickMode)
			{
			case 0:
			{
				posX = Player->GetState().Gamepad.sThumbLX;
				posY = Player->GetState().Gamepad.sThumbLY;
				break;
			}
			case 1:
			{
				posX = Player->GetState().Gamepad.sThumbRX;
				posY = Player->GetState().Gamepad.sThumbLY;
				break;
			}
			case 2:
			{
				posX = Player->GetState().Gamepad.sThumbLX;
				posY = Player->GetState().Gamepad.sThumbRY;
				break;
			}
			case 3:
			{
				posX = Player->GetState().Gamepad.sThumbRX;
				posY = Player->GetState().Gamepad.sThumbRY;
				break;
			}
			default:
			{
				posX = Player->GetState().Gamepad.sThumbLX;
				posY = Player->GetState().Gamepad.sThumbLY;
				break;
			}
			}

			motorsControll(posX, posY, &message[1], &message[2], &message[3], &message[4]);

			serial.cSend(message, 6);

			g_lock.unlock();

			Sleep(SLEEP_MSEC);
		} while (!flgStop);
		break;
	}
	case 1:
	{
		vector<int> lMotorSpeed;
		vector<int> rMotorSpeed;

		vector<bool> lMotorDir;
		vector<bool> rMotorDir;

		char buff[50];

		time_t rawtime;
		struct tm timeinfo;
		time(&rawtime);
		localtime_s(&timeinfo, &rawtime);

		strftime(buff, 50, "-%B-%A-%H-%M", &timeinfo);
		string filePatch = "recordinPatch";
		filePatch += buff;
		filePatch += ".txt";
		cout << filePatch << endl;
		ofstream recordinPatch(filePatch);

		recordinPatch << SLEEP_MSEC << endl;

		do
		{
			g_lock.lock();

			switch (stickMode)
			{
			case 0:
			{
				posX = Player->GetState().Gamepad.sThumbLX;
				posY = Player->GetState().Gamepad.sThumbLY;
				break;
			}
			case 1:
			{
				posX = Player->GetState().Gamepad.sThumbRX;
				posY = Player->GetState().Gamepad.sThumbLY;
				break;
			}
			case 2:
			{
				posX = Player->GetState().Gamepad.sThumbLX;
				posY = Player->GetState().Gamepad.sThumbRY;
				break;
			}
			case 3:
			{
				posX = Player->GetState().Gamepad.sThumbRX;
				posY = Player->GetState().Gamepad.sThumbRY;
				break;
			}
			default:
			{
				posX = Player->GetState().Gamepad.sThumbLX;
				posY = Player->GetState().Gamepad.sThumbLY;
				break;
			}
			}

			if (flgRecording == RECORDING_STOP)
			{
				recordinPatch << lMotorDir.size() << endl;

				for (int i = 0;i < lMotorDir.size();i++)
				{
					recordinPatch << lMotorDir[i] << " ";
					recordinPatch << rMotorDir[i] << " ";
					recordinPatch << lMotorSpeed[i] << " ";
					recordinPatch << rMotorSpeed[i] << endl;
				}
				flgRecording == RECORDING_DEFAULT;
				recordinPatch.close();
			}
			g_lock.unlock();

			motorsControll(posX, posY, &message[1], &message[2], &message[3], &message[4]);

			serial.cSend(message, 6);

			if (flgRecording == RECORDING_START)
			{
				lMotorDir.insert(lMotorDir.end(), message[1]);
				rMotorDir.insert(rMotorDir.end(), message[2]);
				lMotorSpeed.insert(lMotorSpeed.end(), message[3]);
				rMotorSpeed.insert(rMotorSpeed.end(), message[4]);
			}

			Sleep(SLEEP_MSEC);
		} while (!flgStop);

		break;
	}
	case 2:
	{
		vector<int> lMotorSpeed;
		vector<int> rMotorSpeed;

		vector<bool> lMotorDir;
		vector<bool> rMotorDir;

		int size;
		ifstream replay(replayPatch);

		replay >> SLEEP_MSEC;
		replay >> size;

		for (int i = 0;i < size;i++)
		{
			int buff;
			replay >> buff;
			lMotorDir.insert(lMotorDir.end(), buff);

			replay >> buff;
			rMotorDir.insert(rMotorDir.end(), buff);

			replay >> buff;
			lMotorSpeed.insert(lMotorSpeed.end(), buff);

			replay >> buff;
			rMotorSpeed.insert(rMotorSpeed.end(), buff);
		}
		replay.close();

		for (int i = 0;i < size;i++)
		{
			cls();

			SetColor(White);
			cout << "Work mode: " << workMode << endl;
			SetColor(LightGray);

			message[1] = lMotorDir[i];
			message[2] = rMotorDir[i];
			message[3] = lMotorSpeed[i];
			message[4] = rMotorSpeed[i];
			serial.cSend(message, 6);

			SetColor(Cyan);
			cout << "----------------------------------" << endl;

			SetColor(Green);
			cout << setw(3) << left << (int)(message[1]);
			SetColor(LightGray);
			cout << " - "; 
			SetColor(Green);
			cout << setw(3) << left << (int)(message[2]);
			SetColor(LightGray);
			cout << " - ";
			SetColor(Green);
			cout << setw(3) << left << (int)(message[3]);
			SetColor(LightGray);
			cout << " - ";
			SetColor(Green);
			cout << setw(3) << left << (int)(message[4]) << endl;

			SetColor(Cyan);
			cout << "----------------------------------" << endl;

			//-------------------------------------
			int x = (i * 20) / (size);

			char indicator[21];

			for (int a = 0;a < 20;a++)
			{
				if (a < x)
					indicator[a] = '=';
				else if (a == x)
					indicator[a] = '>';
				else
					indicator[a] = ' ';

			}
			indicator[20] = '\0';

			SetColor(White);
			cout << "\r" << "Loading: "; 
			SetColor(Green);
			cout << indicator;
			SetColor(White);
			cout << "| " << setw(3) << left << (int)((float)(i) / (size - 1) * 100) << "%";
			SetColor(LightGray);
			//-------------------------------------

			Sleep(SLEEP_MSEC);
		}
		cout << endl;
		
		SetColor(Cyan);
		cout << "----------------------------------" << endl;
		SetColor(LightGray);

		break;
	}
	}
}

void enterGamepadMode(int *nmbGmpd, int *sMode)
{
	bool flg;
	do {
		SetColor(White);
		std::cout << "Enter number of gamepad(1-4): ";
		SetColor(Yellow);
		std::cin >> *nmbGmpd;

		if (*nmbGmpd < 1 || *nmbGmpd > 4)
		{
			SetColor(Red);
			cout << "----------------------------------" << endl;
			SetColor(LightRed);
			std::cout << "\aincorrect number\n";
			SetColor(Red);
			cout << "----------------------------------" << endl;
			SetColor(LightGray);

			delayAndCls();
			flg = true;
		}
		else flg = false;

	} while (flg);

	SetColor(Cyan);
	cout << "----------------------------------" << endl;

	SetColor(White);
	std::cout << "Control mode: " << std::endl;
	SetColor(LightGray);

	std::cout << "0-Left stick" << std::endl;
	std::cout << "1-Left(Y axis) and Right(X axis) stick)" << std::endl;
	std::cout << "2-Left(X axis) and Right(Y axis) stick)" << std::endl;
	std::cout << "3-Right stick" << std::endl;
	
	SetColor(Cyan);
	cout << "----------------------------------" << endl;
	
	SetColor(White);
	std::cout << "Enter Stick mode: ";
	SetColor(Yellow);
	std::cin >> *sMode;

	SetColor(LightGray);
}
