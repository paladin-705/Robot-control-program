#include "SendMessageThread.h"

using namespace std;

bool flgStop;		
int flgRecording;
string replayPatch;
mutex g_lock;

//Motor controll:
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
				*speedRight = MAX_SPEED_ARDUINO * sqrt(2 * posY - pow(posY, 2));
				coefSpeedDif = sqrt(1 - pow(posX, 2));
				*speedLeft = *speedRight *coefSpeedDif;
			}
			else
			{
				*speedRight = MAX_SPEED_ARDUINO * posY;
				coefSpeedDif = sqrt(1 - pow(posX, 2));
				*speedLeft = *speedRight *coefSpeedDif;
			}
		}
		else {
			if (SPEED_MODE == SPEED_MODE_STANDART)
			{
				*speedLeft = MAX_SPEED_ARDUINO * sqrt(2 * posY - pow(posY, 2));
				coefSpeedDif = sqrt(1 - pow(posX, 2));
				*speedRight = *speedLeft*coefSpeedDif;
			}
			else
			{
				*speedLeft = MAX_SPEED_ARDUINO * posY;
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
		int angle = MAX_ANGLE * posX;

		if (posX < 0) {
			*speedRight = (-1)*angle;
			*directionR = BCKWRD;
		}
		else
		{
			*speedRight = angle;
			*directionR = FRWRD;
		}

		*speedLeft = MAX_SPEED_ARDUINO * posY;
		if (buffPosY < 0) {
			*directionL = BCKWRD;
		}
		else if (buffPosY>0) {
			*directionL = FRWRD;
		}
	}
}
void motorsControllLego(LegoMotors *legoMotors, int length, int buffPosX, int buffPosY)
{

	const bool FRWRD_LEGO = true;
	const bool BCKWRD_LEGO = false;

	int speedRight, speedLeft, angle;
	int directionL, directionR, directionA;

	float posX = fabs(buffPosX / (float)(GAMEPAD_MAXPOS));
	float posY = fabs(buffPosY / (float)(GAMEPAD_MAXPOS));

	if (fabs(buffPosX) < 129)posX = 0;
	if (fabs(buffPosY) < 129)posY = 0;

	//---------Set speed------------------
	float coefSpeedDif;

	if (buffPosX < 0) {
		if (SPEED_MODE == SPEED_MODE_STANDART)
		{
			speedRight = MAX_SPEED_LEGO * sqrt(2 * posY - pow(posY, 2));
			coefSpeedDif = sqrt(1 - pow(posX, 2));
			speedLeft = speedRight *coefSpeedDif;
		}
		else
		{
			speedRight = MAX_SPEED_LEGO * posY;
			coefSpeedDif = sqrt(1 - pow(posX, 2));
			speedLeft = speedRight *coefSpeedDif;
		}
	}
	else {
		if (SPEED_MODE == SPEED_MODE_STANDART)
		{
			speedLeft = MAX_SPEED_LEGO * sqrt(2 * posY - pow(posY, 2));
			coefSpeedDif = sqrt(1 - pow(posX, 2));
			speedRight = speedLeft*coefSpeedDif;
		}
		else
		{
			speedLeft = MAX_SPEED_LEGO * posY;
			coefSpeedDif = sqrt(1 - pow(posX, 2));
			speedRight = speedLeft*coefSpeedDif;
		}
	}
	//Trigger 
	if ((fabs(buffPosY) <= MAX_TRIG_POS_Y) && (fabs(buffPosX) >= MIN_TRIG_POS_X)) {
		if (buffPosX < 0) {
			directionL = BCKWRD_LEGO;
			directionR = FRWRD_LEGO;
			speedLeft = LEGO_SPEED_TRIG;
			speedRight = LEGO_SPEED_TRIG;
		}
		else {
			directionL = FRWRD_LEGO;
			directionR = BCKWRD_LEGO;
			speedLeft = LEGO_SPEED_TRIG;
			speedRight = LEGO_SPEED_TRIG;
		}
	}
	else {
		if (buffPosY < 0) {
			directionL = BCKWRD_LEGO;
			directionR = BCKWRD_LEGO;
		}
		else if (buffPosY>0) {
			directionL = FRWRD_LEGO;
			directionR = FRWRD_LEGO;
		}
	}

	//-----------Turning------------------
	angle = MAX_ANGLE * posX;

	if (posX < 0) {
		speedRight = (-1)*angle;
		directionA = BCKWRD_LEGO;
	}
	else
	{
		speedRight = angle;
		directionA = FRWRD_LEGO;
	}

	speedLeft = MAX_SPEED_LEGO * posY;
	if (buffPosY < 0) {
		directionA = BCKWRD_LEGO;
	}
	else if (buffPosY>0) {
		directionA = FRWRD_LEGO;
	}
	//------------------------------------

	if (directionL == BCKWRD_LEGO) speedLeft *= (-1);
	if (directionR == BCKWRD_LEGO) speedRight *= (-1);
	if (directionA == BCKWRD_LEGO) angle *= (-1);

	for (int i = 0;i < length;i++)
	{
		if (legoMotors->getMotorState())
		{
			if (legoMotors->getRole() == Left)
			{
				if (legoMotors->getInvertState())
					legoMotors->setSpeed(((-1)*speedLeft));
				else
					legoMotors->setSpeed(speedLeft);
			}
			if (legoMotors->getRole() == Right)
			{
				if (legoMotors->getInvertState())
					legoMotors->setSpeed((-1)*(speedRight));
				else
					legoMotors->setSpeed(speedRight);
			}
			if (legoMotors->getRole() == Turning)
			{
				if (legoMotors->getInvertState())
					legoMotors->setAngle((-1)*(angle));
				else
					legoMotors->setAngle(angle);
			}
		}
		
		legoMotors++;
	}
}

//Send Message Thread:
void sendMessageThread(CXBOXController *Player, Serial *serial, int stickMode, int workMode, int programMode)
{
	int posX;
	int posY;

	if (programMode == ARDUINO_MODE)
	{
		int MESSAGE_HEADER;
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

				serial->cSend(message, (sizeof(message) / (sizeof(message[0]))));

				g_lock.unlock();

				Sleep(SLEEP_MSEC);
			} while (!flgStop);
			break;
		}
		case 1:
		{
			//SLEEP_MSEC
			//MODE
			//HEADER
			//LENGTH
			//COMMANDS

			vector<int> recPosX;
			vector<int> recPosY;

			char buff[50];

			time_t rawtime;
			struct tm timeinfo;
			time(&rawtime);
			localtime_s(&timeinfo, &rawtime);

			strftime(buff, 50, "-%B-%A-%H-%M", &timeinfo);
			string filePatch = "recordingPatch";
			filePatch += buff;
			filePatch += ".txt";
			ofstream recordinPatch(filePatch);

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
					recordinPatch << SLEEP_MSEC << endl;
					recordinPatch << ARDUINO_MODE << endl;
					recordinPatch << MESSAGE_HEADER << endl;
					recordinPatch << recPosX.size() << endl;

					for (int i = 0;i < recPosX.size();i++)
					{
						recordinPatch << recPosX[i] << " ";
						recordinPatch << recPosY[i] << endl;
					}
					flgRecording == RECORDING_DEFAULT;
					recordinPatch.close();
				}
				g_lock.unlock();

				motorsControllArduino(posX, posY, &message[1], &message[2], &message[3], &message[4]);

				serial->cSend(message, (sizeof(message) / (sizeof(message[0]))));

				if (flgRecording == RECORDING_START)
				{
					recPosX.insert(recPosX.end(), posX);
					recPosY.insert(recPosY.end(), posY);
				}

				Sleep(SLEEP_MSEC);
			} while (!flgStop);

			break;
		}
		case 2:
		{
			unsigned long long int time = 0;
			unsigned long long int prev_time = 0;

			vector<int>recPosX;
			vector<int>recPosY;

			int size;
			int mode;
			ifstream replay(replayPatch);
			
			replay >> SLEEP_MSEC;
			replay >> mode;

			if (mode != ARDUINO_MODE)
			{
				coutMessage(Error, "This is LEGO replay");
				coutMessage(EnterData, "Continue? (y/n): ");
				string buff;
				cin >> buff;

				if (!(buff == "y" || buff == "Y"))
					break;
			}

			if(mode != ARDUINO_MODE)
				MESSAGE_HEADER = MESSAGE_HEADER_PROPORTIONAL;
			else
				replay >> MESSAGE_HEADER;

			message[0] = MESSAGE_HEADER;
			replay >> size;

			for (int i = 0;i < size;i++)
			{
				int buff;
				replay >> buff;
				recPosX.insert(recPosX.end(), buff);

				replay >> buff;
				recPosY.insert(recPosY.end(), buff);
			}
			replay.close();

			for (int i = 0;i < size;i++)
			{
				cls();

				SetColor(White);
				cout << "Work mode: " << workMode << endl;
				SetColor(LightGray);

				motorsControllArduino(recPosX[i], recPosY[i], &message[1], &message[2], &message[3], &message[4]);
				serial->cSend(message, (sizeof(message) / (sizeof(message[0]))));

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
				cout << setw(3) << left << (int)(message[4]);
					
				time = clock();
				cout << " delay: " << (time - prev_time) - 100 << endl;	//Why -100? Just a planned 
				prev_time = time;

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
		const int LEGO_MASS_LEN = 4;
		LegoMotors *legoMotors = new LegoMotors[LEGO_MASS_LEN];

		loadLegoSetting(legoMotors, LEGO_MASS_LEN);

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

				motorsControllLego(legoMotors, LEGO_MASS_LEN, posX, posY);

				for (int i = 0;i < LEGO_MASS_LEN;i++)
				{
					if (legoMotors[i].getMotorState())
					{
						if (legoMotors[i].getRole() == Turning)
						{
							unsigned char message[] = { 13, 0, 0, 0,
								DIRECT_COMMAND_NO_REPLY,
								0, 0,
								opOUTPUT_POSITION, LC0(0), LC0(legoMotors[i].getMotorNumb()), LC1(legoMotors[i].getSpeed()),
								opOUTPUT_START, LC0(0), LC0(legoMotors[i].getMotorNumb()) };

							serial->cSend(message, (sizeof(message) / (sizeof(message[0]))));
						}
						else
						{
							unsigned char message[] = { 13, 0, 0, 0,
								DIRECT_COMMAND_NO_REPLY,
								0, 0,
								opOUTPUT_SPEED, LC0(0), LC0(legoMotors[i].getMotorNumb()), LC1(legoMotors[i].getSpeed()),
								opOUTPUT_START, LC0(0), LC0(legoMotors[i].getMotorNumb()) };

							serial->cSend(message, (sizeof(message) / (sizeof(message[0]))));
						}
					}
				}

				g_lock.unlock();

				Sleep(SLEEP_MSEC);
			} while (!flgStop);
			break;
		}
		case 1:
		{
			vector<int> recPosX;
			vector<int> recPosY;

			char buff[50];

			time_t rawtime;
			struct tm timeinfo;
			time(&rawtime);
			localtime_s(&timeinfo, &rawtime);

			strftime(buff, 50, "-%B-%A-%H-%M", &timeinfo);
			string filePatch = "recordingPatch";
			filePatch += buff;
			filePatch += ".txt";
			ofstream recordinPatch(filePatch);

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

					recordinPatch << SLEEP_MSEC << endl;
					recordinPatch << LEGO_MODE << endl;
					recordinPatch << recPosX.size() << endl;

					for (int i = 0;i < recPosX.size();i++)
					{
						recordinPatch << recPosX[i] << " ";
						recordinPatch << recPosY[i] << endl;
					}
					flgRecording == RECORDING_DEFAULT;
					recordinPatch.close();
				}
				g_lock.unlock();

				motorsControllLego(legoMotors, LEGO_MASS_LEN, posX, posY);

				for (int i = 0;i < LEGO_MASS_LEN;i++)
				{
					if (legoMotors[i].getMotorState())
					{
						if (legoMotors[i].getRole() == Turning)
						{
							unsigned char message[] = { 13, 0, 0, 0,
								DIRECT_COMMAND_NO_REPLY,
								0, 0,
								opOUTPUT_POSITION, LC0(0), LC0(legoMotors[i].getMotorNumb()), LC1(legoMotors[i].getSpeed()),
								opOUTPUT_START, LC0(0), LC0(legoMotors[i].getMotorNumb()) };

							serial->cSend(message, (sizeof(message) / (sizeof(message[0]))));
						}
						else
						{
							unsigned char message[] = { 13, 0, 0, 0,
								DIRECT_COMMAND_NO_REPLY,
								0, 0,
								opOUTPUT_SPEED, LC0(0), LC0(legoMotors[i].getMotorNumb()), LC1(legoMotors[i].getSpeed()),
								opOUTPUT_START, LC0(0), LC0(legoMotors[i].getMotorNumb()) };

							serial->cSend(message, (sizeof(message) / (sizeof(message[0]))));
						}
					}
				}

				if (flgRecording == RECORDING_START)
				{
					recPosX.insert(recPosX.end(), posX);
					recPosY.insert(recPosY.end(), posY);
				}

				Sleep(SLEEP_MSEC);
			} while (!flgStop);

			break;
		}
		case 2:
		{
			unsigned long long int time = 0;
			unsigned long long int prev_time = 0;

			vector<int>recPosX;
			vector<int>recPosY;

			int size;
			int header;
			int mode;
			ifstream replay(replayPatch);

			replay >> SLEEP_MSEC;

			replay >> mode;
			if (mode != LEGO_MODE)
			{
				coutMessage(Error, "This is Arduino replay");
				coutMessage(EnterData, "Continue? (y/n): ");
				string buff;
				cin >> buff;

				if (!(buff == "y" || buff == "Y"))
					break;
			}

			if (mode != LEGO_MODE)
				replay >> header;

			replay >> size;

			for (int i = 0;i < size;i++)
			{
				time = clock();

				int buff;
				replay >> buff;
				recPosX.insert(recPosX.end(), buff);

				replay >> buff;
				recPosY.insert(recPosY.end(), buff);

			}
			replay.close();

			for (int i = 0;i < size;i++)
			{
				time = clock();

				cls();

				SetColor(White);
				cout << "Work mode: " << workMode << endl;
				SetColor(LightGray);

				motorsControllLego(legoMotors, LEGO_MASS_LEN, recPosX[i], recPosY[i]);

				for (int i = 0;i < LEGO_MASS_LEN;i++)
				{
					if (legoMotors[i].getMotorState())
					{
						if (legoMotors[i].getRole() == Turning)
						{
							unsigned char message[] = { 13, 0, 0, 0,
								DIRECT_COMMAND_NO_REPLY,
								0, 0,
								opOUTPUT_POSITION, LC0(0), LC0(legoMotors[i].getMotorNumb()), LC1(legoMotors[i].getSpeed()),
								opOUTPUT_START, LC0(0), LC0(legoMotors[i].getMotorNumb()) };

							serial->cSend(message, (sizeof(message) / (sizeof(message[0]))));
						}
						else
						{
							unsigned char message[] = { 13, 0, 0, 0,
								DIRECT_COMMAND_NO_REPLY,
								0, 0,
								opOUTPUT_SPEED, LC0(0), LC0(legoMotors[i].getMotorNumb()), LC1(legoMotors[i].getSpeed()),
								opOUTPUT_START, LC0(0), LC0(legoMotors[i].getMotorNumb()) };

							serial->cSend(message, (sizeof(message) / (sizeof(message[0]))));
						}
					}
				}

				coutMessage(Line);

				for (int i = 0;i < LEGO_MASS_LEN;i++)
				{
					if (legoMotors[i].getRole() == Turning)
						cout << setw(3) << left << legoMotors[i].getAngle() << " | ";
					else
						cout << setw(3) << left << legoMotors[i].getSpeed() << " | ";
				}

				time = clock();
				cout << "delay: " << (time - prev_time) - 100 << endl; //Why -100? Just a planned 
				prev_time = time;


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

				prev_time = time;
			}
			cout << endl;

			coutMessage(Line);

			break;
		}
		}

		delete[] legoMotors;
	}

	Player = 0;
	serial = 0;
}
void stopSendMessageThread()
{
	g_lock.lock();
	flgStop = true;
	g_lock.unlock();
}