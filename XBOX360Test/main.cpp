/*
Шаблон сообщения:
S_1_;_2_;_3_n
_1_ - длина части сообщения с координатами положения джойстика (т.е от _2_ до n включительно)
_2_ - позиция стика геймпада по оси X
_3_ - позиция стика геймпада по оси Y
*/


#include "CXBOXController.h"		//Библиотека работы с геймпадом, с сайта http://www.codeproject.com/Articles/26949/Xbox-Controller-Input-in-C-with-XInput
#include "Functions.h"

#include <iostream>					
#include <iomanip> 

#include <cstdlib>

#include <Windows.h>				//Библиотека для работы с функциями перевода каретки на нужную строку
#include <process.h>				//Библиотека для работы с несколькими потоками

CXBOXController* Player1;			//Обьект для работы с геймпадом

//COM порт:
HANDLE hSerial;						//Обработчик COM порта
wchar_t buffPortName[7];			//Временная переменная хранения имени COM порта

bool flgOK = false;					//Флаг нормальной работы потока
bool flgStop = false;				//Флаг окончания работы потока

int STICK_MODE;						//Определяет режим управления (один или два стика)

const int SLEEP_MSEC = 100;			//Задержка между отправками сообщений

void Thread(void* pParams)			//Поток для передачи данных к Arduino
{
	//Переменные для хранения координат:
	int *iPosLX = new int;
	int *iPosLY = new int;
	int *iPosRX = new int;
	int *iPosRY = new int;

	int lenLX, lenLY, lenRX, lenRY;	//Переменные для хранения длинны числа

	while (true)
	{
		if (flgStop) break;			//Если сигнал выхода из потока станет истинным то закрываем поток

		if (flgOK)					//Если инициализация прошла нормально то начинаем деятельность потока
		{
			int dataLen = 0;

			switch (STICK_MODE)
			{
			case 0:
			{
				*iPosLX = Player1->GetState().Gamepad.sThumbLX;
				*iPosLY = Player1->GetState().Gamepad.sThumbLY;
				break;
			}
			case 1:
			{
				*iPosLX = Player1->GetState().Gamepad.sThumbRX;
				*iPosLY = Player1->GetState().Gamepad.sThumbLY;
			}
			case 3:
			{
				*iPosLX = Player1->GetState().Gamepad.sThumbLX;
				*iPosLY = Player1->GetState().Gamepad.sThumbRY;
			}
			default:
			{
				*iPosLX = Player1->GetState().Gamepad.sThumbLX;
				*iPosLY = Player1->GetState().Gamepad.sThumbLY;
				break;
			}
			}

			lenLX = intlen(*iPosLX);
			lenLY = intlen(*iPosLY);

			char *posLX = new char[lenLX+1];
			char *posLY = new char[lenLY+1];

			itoa(*iPosLX, posLX, 10);
			itoa(*iPosLY, posLY, 10);

			dataLen = lenLX + lenLY + 3;

			char *len = new char[intlen(dataLen)];
			itoa(dataLen, len, 10);

			char *data = new char[dataLen + 2 + intlen(dataLen)+1];
			
			//Формируем сообщение:
			strcpy(data, "S");
			strcat(data, len);
			strcat(data, ";");
			
			strcat(data, posLX);
			strcat(data, ";");
			
			strcat(data, posLY);
			strcat(data, "n");
			
			//Передаём данные:
			DWORD dwSize = sizeof(char)*(dataLen + 2 + intlen(dataLen) + 1);		 // размер передаваемой строки
			DWORD dwBytesWritten;													 // тут будет количество собственно переданных байт
			
			BOOL iRet = WriteFile(hSerial, data, dwSize, &dwBytesWritten, NULL);

			delete[] data;
			delete[] posLX;
			delete[] posLY;

			Sleep(SLEEP_MSEC);
		}
	}

	//Удаление переменных для хранения координат:
	delete iPosLX;
	delete iPosLY;
	delete iPosRX;
	delete iPosRY;

	_endthread();					//Закрываем поток
}

int main(int argc, char* argv[])
{
	int nmbGmpd;					//Переменная для хранения номера геймпада

	/*
		To Do: добавть try-catch блок вместо if условий проверки введённых данных
	*/

error:								//Если введённое значение несоответсвует условию то его надо ввести по новой
	
	std::cout << "Enter number of gamepad(1-4): ";
	std::cin >> nmbGmpd;

	std::cout << "Control mode: " << std::endl;
	std::cout << "0-Left stick" << std::endl;
	std::cout << "1-Left(Y axis) and Right(X axis) stick)" << std::endl;
	std::cout << "2-Left(X axis) and Right(Y axis) stick)" << std::endl;
	std::cout << "Enter Stick mode: ";
	std::cin >> STICK_MODE;

	std::cout << "Enter COM port number(COM_): ";
	std::wcin >> buffPortName;

	//Работа с COM портом:
	LPCTSTR sPortName = buffPortName;

	hSerial = ::CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			std::cout << "serial port does not exist.\n";
		}
		std::cout << "some other error occurred.\n";

		system("pause");
		system("cls");
		goto error;
	}

	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams))
	{
		std::cout << "getting state error\n";

		system("pause");
		system("cls");
		goto error;
	}
	dcbSerialParams.BaudRate = CBR_9600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!SetCommState(hSerial, &dcbSerialParams))
	{
		std::cout << "error setting serial port state\n";

		system("pause");
		system("cls");
		goto error;
	}

	//----------------------------------------------------------------------

	if (nmbGmpd < 1 || nmbGmpd > 4)
	{
		std::cout << "\aincorrect number\n";
		system("pause");
		system("cls");
		goto error;
	}

	Player1 = new CXBOXController(nmbGmpd);

	_beginthread(Thread, 0, NULL);	//Открываем поток для передачи данных на Arduino

	std::cout << "Instructions:\n";
	std::cout << "[BACK] Exit\n";
	
	while(true)
	{
		if(Player1->IsConnected())
		{
			flgOK = true;			//Сигнал начала работы второго потока в нормальном режиме

			cls();					//Очищаем экран
			
			//Выводим информацию о координатах стиков геймпада:
			std::cout << "Left stick:\n";
			std::cout << "X pos: " << std::setw(6) << Player1->GetState().Gamepad.sThumbLX << " " << "Y pos: " << std::setw(6) << Player1->GetState().Gamepad.sThumbLY << std::endl;
			std::cout << "Right stick:\n";
			std::cout << "X pos: " << std::setw(6) << Player1->GetState().Gamepad.sThumbRX << " " << "Y pos: " << std::setw(6) << Player1->GetState().Gamepad.sThumbRY << std::endl;
			
			if(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
			{
				flgStop = true;		//Закрываем второй поток
				break;
			}
		}
		else
		{
			std::cout << "\t\aERROR! PLAYER " << nmbGmpd << " - XBOX 360 Controller Not Found!\n";
			std::cout << "Press Any Key To Exit.\n";
			flgStop = true;			//Закрываем второй поток
			system("pause");
			break;
		}
	}
	delete(Player1);				//Удаляем обьект Player1
	return( 0 );
}