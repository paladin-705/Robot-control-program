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
		string standardName[4] = {
			"MOTOR_A:",
			"MOTOR_B:",
			"MOTOR_C:",
			"MOTOR_D:"
		};

		string standardRole[3] = {
			"L",
			"R",
			"A"
		};

		struct LegoMotor
		{
			string name;
			bool state;
			bool inversion;
			string role;
		}legoMotors[4], buffLegoMotor;

		auto loadLegoSettingFunc = [](LegoMotor *legoMotors)
		{
			ifstream settingFileL("legoMotorsSetting.ini");
			for (int i = 0;i < 4;i++)
			{
				settingFileL >> legoMotors[i].name;
				settingFileL >> legoMotors[i].state;
				settingFileL >> legoMotors[i].inversion;
				settingFileL >> legoMotors[i].role;
			}
			settingFileL.close();
		};

		loadLegoSettingFunc(legoMotors);

		for (int i = 0;i < 4;i++)
		{
			bool flg = false;
			for (int a = 0;a < 4;a++)
			{
				if (legoMotors[i].name == standardName[a])
					flg = true;
			}
			if (!flg)
			{
				coutMessage(Error, "Incorrect settings");
				coutMessage(EnterData, "Change to default (y/n)? ");
				string buff;
				cin >> buff;

				if (buff == "y" || buff == "Y")
				{
					ofstream settingFileS("legoMotorsSetting.ini");
					settingFileS << "MOTOR_A: " << 0 << " " << 0 << " " << "L" << endl;
					settingFileS << "MOTOR_B: " << 0 << " " << 1 << " " << "R" << endl;
					settingFileS << "MOTOR_C: " << 1 << " " << 0 << " " << "A" << endl;
					settingFileS << "MOTOR_D: " << 1 << " " << 1 << " " << "A" << endl;
					settingFileS.close();

					system("cls");
					coutMessage(Success, "Settings changed to to default");
					system("pause");

					loadLegoSettingFunc(legoMotors);
				}
				system("cls");
				break;
			}
		}

		for (int i = 0;i < 4;i++)
		{
			bool flg = false;
			for (int a = 0;a < 3;a++)
			{
				if (legoMotors[i].role == standardRole[a])
					flg = true;
			}
			if (!flg)
			{
				coutMessage(Error, "Incorrect settings");
				coutMessage(EnterData, "Change to default (y/n)? ");
				string buff;
				cin >> buff;

				if (buff == "y" || buff == "Y")
				{
					ofstream settingFileS("legoMotorsSetting.ini");
					settingFileS << "MOTOR_A: " << 0 << " " << 0 << " " << "L" << endl;
					settingFileS << "MOTOR_B: " << 0 << " " << 1 << " " << "R" << endl;
					settingFileS << "MOTOR_C: " << 1 << " " << 0 << " " << "A" << endl;
					settingFileS << "MOTOR_D: " << 1 << " " << 1 << " " << "A" << endl;
					settingFileS.close();

					system("cls");
					coutMessage(Success, "Settings changed to to default");
					system("pause");

					loadLegoSettingFunc(legoMotors);
				}
				system("cls");
				break;
			}
		}

		while (true)
		{
			SetColor(LightGray);
			cout << "Enter END if you want exit" << endl;
			SetColor(Yellow);

			ofstream settingFileS("legoMotorsSetting.ini");
			for (int i = 0;i < 4;i++)
			{
				cout << legoMotors[i].name << " " << legoMotors[i].state << " " << legoMotors[i].inversion << " " << legoMotors[i].role << '\n';
				settingFileS << legoMotors[i].name << " " << legoMotors[i].state << " " << legoMotors[i].inversion << " " << legoMotors[i].role << '\n';
			}
			settingFileS.close();

			coutMessage(EnterData, "Enter new setting: ");
			cin >> buffLegoMotor.name;
			if (buffLegoMotor.name == "END") break;
			cin >> buffLegoMotor.state;
			cin >> buffLegoMotor.inversion;
			cin >> buffLegoMotor.role;

			for (int i = 0;i < 4;i++)
			{
				if (buffLegoMotor.name == legoMotors[i].name)
				{
					legoMotors[i] = buffLegoMotor;
					break;
				}
			}
			
			system("cls");
		}

		break;
	}
	case 1:
	{
		string settingName[13] = {
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
			"FRWRD:",
			"OPTIMIZE_PATCH:"
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

				for (int i = 0;i < 13;i++)
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

		settingFileS << "OPTIMIZE_PATCH:" << " " << DEFAULT_OPTIMIZE_PATCH << endl;

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

	OPTIMIZE_PATCH = DEFAULT_OPTIMIZE_PATCH;
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
		if (motorNumb == "MOTOR_A:")	legoMotors->setMotorNumb(MotorA);
		if (motorNumb == "MOTOR_B:")	legoMotors->setMotorNumb(MotorB);
		if (motorNumb == "MOTOR_C:")	legoMotors->setMotorNumb(MotorC);
		if (motorNumb == "MOTOR_D:")	legoMotors->setMotorNumb(MotorD);

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

		if (varName == "OPTIMIZE_PATCH:") OPTIMIZE_PATCH = value;
	}
	setting.close();
}
