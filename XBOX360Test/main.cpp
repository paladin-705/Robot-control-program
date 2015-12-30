// main.cpp: 
//
#include "Serial lib\Serial.h"
#include "CXBOXController.h"
#include "Functions.h"
#include "Constants.h"

#include <iostream>
#include <iomanip> 
#include <Windows.h>

#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <map>
#include <iterator>

#include <thread>
#include <mutex>

using namespace std;

//--------------------------
int MESSAGE_HEADER;

int SLEEP_MSEC;

int SPEED_MODE;
int MAX_SPEED;
int ARDUINO_SPEED_TRIG;

int GAMEPAD_MAXPOS;
int MAX_TRIG_POS_Y;
int MIN_TRIG_POS_X;

int MAX_ANGLE;

bool BCKWRD;
bool FRWRD;
//--------------------------

string replayPatch;

mutex g_lock;

Serial serial;
CXBOXController* Player;

bool flgStop;		//Exit on sendMessageThread
int flgRecording;

void changeSetting();
void defaultSetting();
void loadLegoSetting();
void loadSetting();
void enterGamepadMode(int *, int *);
void motorsControll(int, int, unsigned char *, unsigned char *, unsigned char *, unsigned char *);
void sendMessageThread(int, int, int);
void stopSendMessageThread();

int main()
{
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
		loadLegoSetting();
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

		thread sendMesThr(sendMessageThread, sMode, wMode, legoArduinoMode);	//????????? ????? ??? ???????? ?????? ?? Arduino

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
		thread sendMesThr(sendMessageThread, sMode, wMode, legoArduinoMode);
	
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

		thread sendMesThr(sendMessageThread, sMode, wMode, legoArduinoMode);
		sendMesThr.join();
		delayAndCls();
		break;
	}
	}

	stopSendMessageThread();
	serial.close();

	return 0;
}

void changeSetting()
{
	bool settingMode;

	system("cls");
	coutMessage(Header, "Work mode: ");
	cout << "0-Default Setting" << endl;
	cout << "1-Change Setting" << endl;
	coutMessage(EnterData, "Enter Work mode: ");
	cin >> settingMode;
	system("cls");

	if (settingMode)
	{
		string settingName[10] = {
			"SLEEP_MSEC:",
			"SPEED_MODE:",
			"MAX_SPEED:",
			"ARDUINO_SPEED_TRIG:",
			"GAMEPAD_MAXPOS:",
			"MAX_TRIG_POS_Y:",
			"MIN_TRIG_POS_X:",
			"MAX_ANGLE:",
			"BCKWRD:",
			"FRWRD:"
		};

		map <string, int> Setting;

		ifstream settingFileL("setting.ini");
		string varName;
		int value;

		while (!settingFileL.eof())
		{
			settingFileL >> varName;
			settingFileL >> value;

			Setting.insert(pair<string, int>(varName, value));
		}
		settingFileL.close();

		string buffName;
		int buffValue;
		ofstream settingFileS("setting.ini");

		while (true)
		{
			SetColor(LightGray);

			cout << "Enter END if you want exit" << endl;

			auto SettingIter = Setting.begin();
			SetColor(Yellow);
			for (SettingIter; SettingIter != Setting.end(); ++SettingIter)
			{
				cout << SettingIter->first << " " << SettingIter->second << '\n';
				settingFileS << SettingIter->first << " " << SettingIter->second << '\n';

			}

			coutMessage(EnterData, "Enter new setting: ");
			cin >> buffName;
			if (buffName == "END") break;
			cin >> buffValue;

			auto searchSetting = Setting.find(buffName);

			if (searchSetting != Setting.end())
			{
				Setting.at(buffName) = buffValue;
			}
			else
			{
				bool flg = false;

				for (int i = 0;i < 10;i++)
				{
					if (buffName == settingName[i])
					{
						flg = true;
						break;
					}
				}
				if (flg)Setting.insert(Setting.end(), make_pair(buffName, buffValue));
			}
			system("cls");
		}

		settingFileS.close();
	}
	else
	{
		ofstream settingFileS("setting.ini");

		settingFileS << "SLEEP_MSEC:" << " " << DEFAULT_SLEEP_MSEC << endl;

		settingFileS << "SPEED_MODE:" << " " << SPEED_MODE_STANDART << endl;
		settingFileS << "MAX_SPEED:" << " " << DEFAULT_MAX_SPEED << endl;
		settingFileS << "ARDUINO_SPEED_TRIG:" << " " << DEFAULT_ARDUINO_SPEED_TRIG << endl;

		settingFileS << "GAMEPAD_MAXPOS:" << " " << DEFAULT_GAMEPAD_MAXPOS << endl;
		settingFileS << "MAX_TRIG_POS_Y:" << " " << DEFAULT_MAX_TRIG_POS_Y << endl;
		settingFileS << "MIN_TRIG_POS_X:" << " " << DEFAULT_MIN_TRIG_POS_X << endl;

		settingFileS << "MAX_ANGLE:" << " " << DEFAULT_MAX_ANGLE << endl;

		settingFileS << "BCKWRD:" << " " << DEFAULT_BCKWRD << endl;
		settingFileS << "FRWRD:" << " " << DEFAULT_FRWRD << endl;

		settingFileS.close();
		coutMessage(Success, "All settings changed to default");
		system("pause");
	}
}

void defaultSetting()
{
	SLEEP_MSEC = DEFAULT_SLEEP_MSEC;

	SPEED_MODE = SPEED_MODE_STANDART;
	MAX_SPEED = DEFAULT_MAX_SPEED;
	ARDUINO_SPEED_TRIG = DEFAULT_ARDUINO_SPEED_TRIG;
	
	GAMEPAD_MAXPOS = DEFAULT_GAMEPAD_MAXPOS;
	MAX_TRIG_POS_Y = DEFAULT_MAX_TRIG_POS_Y;
	MIN_TRIG_POS_X = DEFAULT_MIN_TRIG_POS_X;

	MAX_ANGLE = DEFAULT_MAX_ANGLE;

	BCKWRD = DEFAULT_BCKWRD;
	FRWRD = DEFAULT_FRWRD;
}

void loadLegoSetting()
{

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

		if (varName == "SPEED_MODE:")
		{
			SPEED_MODE = value;
			if (SPEED_MODE < 0 || SPEED_MODE > 2)
				SPEED_MODE = SPEED_MODE_STANDART;
		}
		if (varName == "MAX_SPEED:") MAX_SPEED = value;
		if (varName == "ARDUINO_SPEED_TRIG:") {
			if (value >= 0 || value <= 255) ARDUINO_SPEED_TRIG = value;
			else if (value > 255) ARDUINO_SPEED_TRIG = 255;
			else if (value < 0) ARDUINO_SPEED_TRIG = 0;
		}

		if (varName == "GAMEPAD_MAXPOS:") GAMEPAD_MAXPOS = value;
		if (varName == "MAX_TRIG_POS_Y:") MAX_TRIG_POS_Y = value;
		if (varName == "MIN_TRIG_POS_X:") MIN_TRIG_POS_X = value;
		
		if (varName == "MAX_ANGLE:") MAX_ANGLE = value;

		if (varName == "BCKWRD:") BCKWRD = value;
		if (varName == "FRWRD:") FRWRD = value;
			
	}
	setting.close();
}

void motorsControllArduino(int buffPosX, int buffPosY, unsigned char *directionL, unsigned char *directionR, unsigned char *speedLeft, unsigned char *speedRight)
{
	float posX = fabs(buffPosX / (float)(GAMEPAD_MAXPOS));
	float posY = fabs(buffPosY / (float)(GAMEPAD_MAXPOS));

	if (fabs(buffPosX) < 129)posX = 0;
	if (fabs(buffPosY) < 129)posY = 0;

	if (SPEED_MODE != SPEED_MODE_TURNING)
	{
		float coefSpeedDif;

		if (buffPosX < 0) {
			if (SPEED_MODE == SPEED_MODE_STANDART)
			{
				*speedRight = MAX_SPEED * sqrt(2 * posY - pow(posY, 2));
				coefSpeedDif = sqrt(1 - pow(posX, 2));
				*speedLeft = *speedRight *coefSpeedDif;
			}
			else
			{
				*speedRight = MAX_SPEED * posY;
				coefSpeedDif = sqrt(1 - pow(posX, 2));
				*speedLeft = *speedRight *coefSpeedDif;
			}
		}
		else {
			if (SPEED_MODE == SPEED_MODE_STANDART)
			{
				*speedLeft = MAX_SPEED * sqrt(2 * posY - pow(posY, 2));
				coefSpeedDif = sqrt(1 - pow(posX, 2));
				*speedRight = *speedLeft*coefSpeedDif;
			}
			else
			{
				*speedLeft = MAX_SPEED * posY;
				coefSpeedDif = sqrt(1 - pow(posX, 2));
				*speedRight = *speedLeft*coefSpeedDif;
			}
		}
		//Trigger 
		if ((fabs(buffPosY) <= MAX_TRIG_POS_Y) && (fabs(buffPosX) >= MIN_TRIG_POS_X)) {
			if (buffPosX < 0) {
				*directionL = BCKWRD;
				*directionR = FRWRD;
				*speedLeft = ARDUINO_SPEED_TRIG;
				*speedRight = ARDUINO_SPEED_TRIG;
			}
			else {
				*directionL = FRWRD;
				*directionR = BCKWRD;
				*speedLeft = ARDUINO_SPEED_TRIG;
				*speedRight = ARDUINO_SPEED_TRIG;
			}
		}
		else {
			if (buffPosY < 0) {
				*directionL = BCKWRD;
				*directionR = BCKWRD;
			}
			else if (buffPosY>0) {
				*directionL = FRWRD;
				*directionR = FRWRD;
			}
		}
	}
	else
	{
		static int prevAngle;
		int diffAngle = (MAX_ANGLE * posX) - prevAngle;
		prevAngle = MAX_ANGLE * posX;

		if (posX < 0) {
			*speedRight = (-1)*diffAngle;
			*directionR = BCKWRD;
		}
		else
		{
			*speedRight = diffAngle;
			*directionR = FRWRD;
		}

		*speedLeft = MAX_SPEED * posY;
		if (buffPosY < 0) {
			*directionL = BCKWRD;
		}
		else if (buffPosY>0) {
			*directionL = FRWRD;
		}
	}
}

void sendMessageThread(int stickMode, int workMode, int programMode)
{
	int posX;
	int posY;

	if (programMode == ARDUINO_MODE)
	{

		unsigned char message[6];
		/*
		0 - header
		1 - left motor direction
		2 - right motor direction
		3 - left motor speed
		4 - right motor speed
		5 - end
		*/

		/*
		0 - header
		1 - motors direction
		2 - angle direction
		3 - speed
		4 - angle
		5 - end
		*/
		switch (SPEED_MODE)
		{
		case SPEED_MODE_PROPORTIONAL:
		{
			MESSAGE_HEADER = MESSAGE_HEADER_PROPORTIONAL;
			break;
		}
		case SPEED_MODE_STANDART:
		{
			MESSAGE_HEADER = MESSAGE_HEADER_STANDART;
			break;
		}
		case SPEED_MODE_TURNING:
		{
			MESSAGE_HEADER = MESSAGE_HEADER_TURNING;
			break;
		}
		}
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

				motorsControllArduino(posX, posY, &message[1], &message[2], &message[3], &message[4]);

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

			unsigned long long int start_time = 0;
			unsigned long long int stop_time = 0;

			time_t rawtime;
			struct tm timeinfo;
			time(&rawtime);
			localtime_s(&timeinfo, &rawtime);

			strftime(buff, 50, "-%B-%A-%H-%M", &timeinfo);
			string filePatch = "recordingPatch";
			filePatch += buff;
			filePatch += ".txt";
			ofstream recordinPatch(filePatch);

			bool startTimerFlg = true;

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
					stop_time = clock();

					recordinPatch << (stop_time - start_time) / lMotorDir.size() << endl;
					recordinPatch << ARDUINO_MODE << endl;
					recordinPatch << MESSAGE_HEADER << endl;
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

				motorsControllArduino(posX, posY, &message[1], &message[2], &message[3], &message[4]);

				serial.cSend(message, 6);

				if (flgRecording == RECORDING_START)
				{
					if (startTimerFlg)
					{
						start_time = clock();
						startTimerFlg = false;
					}
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
			int mode;
			ifstream replay(replayPatch);

			replay >> SLEEP_MSEC;
			replay >> mode;

			if (mode != ARDUINO_MODE)
			{
				coutMessage(Error, "This is LEGO replay");
				break;
			}

			replay >> MESSAGE_HEADER;
			message[0] = MESSAGE_HEADER;
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

				coutMessage(Line);

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

				coutMessage(Line);

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

			coutMessage(Line);

			break;
		}
		}
	}
	else
	{
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

				//ADD LEGO CODE HERE
				//
				//
				//
				//
				//
				//

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

			unsigned long long int start_time = 0;
			unsigned long long int stop_time = 0;

			time_t rawtime;
			struct tm timeinfo;
			time(&rawtime);
			localtime_s(&timeinfo, &rawtime);

			strftime(buff, 50, "-%B-%A-%H-%M", &timeinfo);
			string filePatch = "recordingPatch";
			filePatch += buff;
			filePatch += ".txt";
			ofstream recordinPatch(filePatch);

			bool startTimerFlg = true;

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
					stop_time = clock();

					recordinPatch << (stop_time - start_time) / lMotorDir.size() << endl;
					recordinPatch << LEGO_MODE << endl;
					recordinPatch << lMotorDir.size() << endl;

					for (int i = 0;i < lMotorDir.size();i++)
					{
						//ADD LEGO CODE HERE
						//
						//
						//
						//
						//
						//
					}
					flgRecording == RECORDING_DEFAULT;
					recordinPatch.close();
				}
				g_lock.unlock();

				//ADD LEGO CODE HERE
				//
				//
				//
				//
				//
				//

				if (flgRecording == RECORDING_START)
				{
					if (startTimerFlg)
					{
						start_time = clock();
						startTimerFlg = false;
					}
					//ADD LEGO CODE HERE
					//
					//
					//
					//
					//
					//
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
			int mode;
			ifstream replay(replayPatch);

			replay >> SLEEP_MSEC;
			
			replay >> mode;
			if (mode != LEGO_MODE)
			{
				coutMessage(Error, "This is Arduino replay");
				break;
			}

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

				//ADD LEGO CODE HERE
				//
				//
				//
				//
				//
				//

				coutMessage(Line);

				//ADD LEGO CODE HERE
				//
				//
				//
				//
				//
				//

				coutMessage(Line);

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

			coutMessage(Line);

			break;
		}
		}
	}
}

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

void stopSendMessageThread()
{
	g_lock.lock();
	flgStop = true;
	g_lock.unlock();
}