#include "SettingFunc.h"

using namespace std;

//Setting:
void changeSetting()
{
	int settingMode;

	system("cls");
	coutMessage(Header, "Work mode: ");
	cout << "0-Default Setting" << endl;
	cout << "1-Change Setting" << endl;
	cout << "2-Change Lego Setting" << endl;
	coutMessage(EnterData, "Enter Work mode: ");
	std::cin >> settingMode;
	system("cls");

	switch (settingMode)
	{
	case 2:
	{

		break;
	}
	case 1:
	{
		string settingName[12] = {
			"SLEEP_MSEC:",
			"SPEED_MODE:",
			"MAX_SPEED_ARDUINO:",
			"MAX_SPEED_LEGO:",
			"ARDUINO_SPEED_TRIG:",
			"LEGO_SPEED_TRIG:",
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

		while (true)
		{
			SetColor(LightGray);

			cout << "Enter END if you want exit" << endl;

			auto SettingIter = Setting.begin();
			SetColor(Yellow);

			ofstream settingFileS("setting.ini");
			for (SettingIter; SettingIter != Setting.end(); ++SettingIter)
			{
				cout << SettingIter->first << " " << SettingIter->second << '\n';
				settingFileS << SettingIter->first << " " << SettingIter->second << '\n';

			}
			settingFileS.close();

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

				for (int i = 0;i < 12;i++)
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

		break;
	}
	case 0:
	{
		ofstream settingFileS("setting.ini");

		settingFileS << "SLEEP_MSEC:" << " " << DEFAULT_SLEEP_MSEC << endl;

		settingFileS << "SPEED_MODE:" << " " << SPEED_MODE_STANDART << endl;
		settingFileS << "MAX_SPEED_ARDUINO:" << " " << DEFAULT_MAX_SPEED_ARDUINO << endl;
		settingFileS << "MAX_SPEED_LEGO:" << " " << DEFAULT_MAX_SPEED_LEGO << endl;
		settingFileS << "ARDUINO_SPEED_TRIG:" << " " << DEFAULT_ARDUINO_SPEED_TRIG << endl;
		settingFileS << "LEGO_SPEED_TRIG:" << " " << DEFAULT_LEGO_SPEED_TRIG << endl;

		settingFileS << "GAMEPAD_MAXPOS:" << " " << DEFAULT_GAMEPAD_MAXPOS << endl;
		settingFileS << "MAX_TRIG_POS_Y:" << " " << DEFAULT_MAX_TRIG_POS_Y << endl;
		settingFileS << "MIN_TRIG_POS_X:" << " " << DEFAULT_MIN_TRIG_POS_X << endl;

		settingFileS << "MAX_ANGLE:" << " " << DEFAULT_MAX_ANGLE << endl;

		settingFileS << "BCKWRD:" << " " << DEFAULT_BCKWRD << endl;
		settingFileS << "FRWRD:" << " " << DEFAULT_FRWRD << endl;

		settingFileS.close();
		coutMessage(Success, "All settings changed to default");
		system("pause");
		break;
	}
	}
}
void defaultSetting()
{
	SLEEP_MSEC = DEFAULT_SLEEP_MSEC;

	SPEED_MODE = SPEED_MODE_STANDART;
	MAX_SPEED_ARDUINO = DEFAULT_MAX_SPEED_ARDUINO;
	MAX_SPEED_LEGO = DEFAULT_MAX_SPEED_LEGO;
	ARDUINO_SPEED_TRIG = DEFAULT_ARDUINO_SPEED_TRIG;
	LEGO_SPEED_TRIG = DEFAULT_LEGO_SPEED_TRIG;

	GAMEPAD_MAXPOS = DEFAULT_GAMEPAD_MAXPOS;
	MAX_TRIG_POS_Y = DEFAULT_MAX_TRIG_POS_Y;
	MIN_TRIG_POS_X = DEFAULT_MIN_TRIG_POS_X;

	MAX_ANGLE = DEFAULT_MAX_ANGLE;

	BCKWRD = DEFAULT_BCKWRD;
	FRWRD = DEFAULT_FRWRD;
}
void loadLegoSetting(LegoMotors *legoMotors, int length)
{
	ifstream setting("legoMotorsSetting.ini");

	string motorNumb;
	bool motorState;
	bool inversion;
	string role;

	for (int i = 0;i < length;i++)
	{
		setting >> motorNumb;
		if (motorNumb == "MOTOR_A")	legoMotors->setMotorNumb(MotorA);
		if (motorNumb == "MOTOR_B")	legoMotors->setMotorNumb(MotorB);
		if (motorNumb == "MOTOR_C")	legoMotors->setMotorNumb(MotorC);
		if (motorNumb == "MOTOR_D")	legoMotors->setMotorNumb(MotorD);

		setting >> motorState;
		legoMotors->setMotorState(motorState);

		setting >> inversion;
		legoMotors->setInvertState(inversion);

		setting >> role;
		if (role == "L")	legoMotors->setRole(Left);
		if (role == "R")	legoMotors->setRole(Right);
		if (role == "A")	legoMotors->setRole(Turning);

		legoMotors++;
	}
	setting.close();
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
		if (varName == "MAX_SPEED_ARDUINO:") {
			MAX_SPEED_ARDUINO = value;
			if (MAX_SPEED_ARDUINO < 0 || MAX_SPEED_ARDUINO > 255)
				MAX_SPEED_ARDUINO = DEFAULT_MAX_SPEED_ARDUINO;
		}
		if (varName == "MAX_SPEED_LEGO:") {
			MAX_SPEED_LEGO = value;
			if (MAX_SPEED_LEGO < 0 || MAX_SPEED_LEGO > 100)
				MAX_SPEED_LEGO = DEFAULT_MAX_SPEED_LEGO;
		}
		if (varName == "ARDUINO_SPEED_TRIG:") {
			if (value >= 0 || value <= 255) ARDUINO_SPEED_TRIG = value;
			else if (value > 255) ARDUINO_SPEED_TRIG = 255;
			else if (value < 0) ARDUINO_SPEED_TRIG = 0;
		}
		if (varName == "LEGO_SPEED_TRIG:") {
			if (value >= 0 || value <= 100) LEGO_SPEED_TRIG = value;
			else if (value > 100) LEGO_SPEED_TRIG = 100;
			else if (value < 0) LEGO_SPEED_TRIG = 0;
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
