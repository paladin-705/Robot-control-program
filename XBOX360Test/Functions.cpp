#include <windows.h>
#include <iostream>	

#include "Functions.h"

void SetColor(ConsoleColorCOUT text)
{
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdOut, (WORD)(text));
}

int intlen(int a)					//Функция определения длинны числа
{
	int len = 0;

	if (a < 0)
	{
		a *= (-1);
		len++;
	}

	while (a > 0)
	{
		a /= 10;
		len++;
	}
	return len;
}

void cls()												//Функция очистки экрана
{
	COORD position;                                     // Объявление необходимой структуры
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Получение дескриптора устройства стандартного вывода

	position.X = 0;										// Установка координаты X
	position.Y = 0;										// Установка координаты Y

	Sleep(100);											//Останавливаем выполнение программы на 100мс для прекращения дёрганья символов на экране

	SetConsoleCursorPosition(hConsole, position);		//Устанавливаем курсор в нужную позицию

														//Очищаем нужные строчки:
	std::cout << "                                          " << std::endl
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