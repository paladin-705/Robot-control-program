#pragma once

#include <Windows.h>
#include <fstream>

class Serial
{
public:
	Serial();
	Serial(wchar_t *);
	Serial(LPCTSTR *);
	~Serial();

	//Functions:
	void setCOMname(wchar_t *);
	void setCOMname(LPCTSTR *);
	int initCOM(void);
	int initCOM(int);

	char cReadCOM();
	void fReadCOM(std::ofstream *);
	int cSend(char *, int);

private:

	//Variables and constants:
	HANDLE serialPort;
	LPCTSTR sPortName;
	const int DEFAULT_BAUDRATE = 9600;
	int baudRate;
};

