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
	int initCOM(void);
	int initCOM(int);
	void setCOMname(wchar_t *);
	void setCOMname(LPCTSTR *);
	void close();

	char cReadCOM();
	void fReadCOM(std::ofstream *);
	int cSend(unsigned char *);
	int cSend(unsigned char *, int);

private:

	//Variables and constants:
	HANDLE serialPort;
	LPCTSTR sPortName;
	const int DEFAULT_BAUDRATE = 9600;
	int baudRate;
};

