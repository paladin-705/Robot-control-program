#include "InputFunctions.h"


void SetColor(ConsoleColorCOUT text)
{
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdOut, (WORD)(text));
}

void coutMessage(MessageType type)
{
	switch (type)
	{
	case Line:
	{
		SetColor(Cyan);
		std::cout << "----------------------------------" << std::endl;
		SetColor(LightGray);
		break;
	}
	}
}

void coutMessage(MessageType type, std::string message)
{
	switch (type)
	{
	case Error:
	{
		SetColor(Red);
		std::cout << "----------------------------------" << std::endl;
		SetColor(LightRed);
		std::cout << "\a" << message << std::endl;
		SetColor(Red);
		std::cout << "----------------------------------" << std::endl;
		SetColor(LightGray);
		break;
	}
	case Success:
	{
		SetColor(Green);
		std::cout << "----------------------------------" << std::endl;
		SetColor(LightGreen);
		std::cout << "\a" << message << std::endl;
		SetColor(Green);
		std::cout << "----------------------------------" << std::endl;
		SetColor(LightGray);
		break;
	}
	case Header:
	{
		SetColor(White);
		std::cout << message << std::endl;
		SetColor(LightGray);
		break;
	}
	case EnterData:
	{
		SetColor(White);
		std::cout << message;
		SetColor(Yellow);
		break;
	}
	case Line:
	{
		SetColor(Cyan);
		std::cout << "----------------------------------" << std::endl;
		SetColor(LightGray);
		break;
	}
	case GamepadData:
	{
		SetColor(Green);
		std::cout << message << std::endl;
		SetColor(White);
		break;
	}
	case RecordingStart:
	{
		SetColor(Green);
		std::cout << message << std::endl;
		SetColor(LightGray);
		break;
	}
	case RecordingStop:
	{
		SetColor(Red);
		std::cout << message << std::endl;
		SetColor(LightGray);
		break;
	}
	}
}

void cls()												//Функция очистки экрана
{
	COORD position;                                     // Объявление необходимой структуры
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Получение дескриптора устройства стандартного вывода

	position.X = 0;										// Установка координаты X
	position.Y = 0;										// Установка координаты Y

	SetConsoleCursorPosition(hConsole, position);		//Устанавливаем курсор в нужную позицию

														//Очищаем нужные строчки:
	std::cout << "                                          " << std::endl
		<< "                                          " << std::endl
		<< "                                          " << std::endl
		<< "                                          " << std::endl
		<< "                                          " << std::endl
		<< "                                          " << std::endl
		<< "                                          " << std::endl;
		
	SetConsoleCursorPosition(hConsole, position);		//Устанавливаем курсор в нужную позицию
}

void delayAndCls()
{
	system("pause");
	system("cls");
}