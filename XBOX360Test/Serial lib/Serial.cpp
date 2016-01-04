#include "Serial.h"

Serial::Serial()
{
	serialPort = 0;
	sPortName = 0;
	baudRate = 0;
}

Serial::Serial(wchar_t *portNumb)
{
	sPortName = portNumb;
}

Serial::Serial(LPCTSTR *portNumb)
{
	sPortName = *portNumb;
}

Serial::~Serial()
{
	CloseHandle(serialPort);
	serialPort = 0;
	sPortName = 0;
	baudRate = 0;
}

void Serial::close()
{
	CloseHandle(serialPort);
	serialPort = 0;
	sPortName = 0;
	baudRate = 0;
}

void Serial::setCOMname(wchar_t *portNumb)
{
	sPortName = portNumb;
}

void Serial::setCOMname(LPCTSTR *portNumb)
{
	sPortName = *portNumb;
}

int Serial::initCOM() 
{
	baudRate = DEFAULT_BAUDRATE;

	serialPort = ::CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (serialPort == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			return 1;
		}
		return 2;
	}

	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	if (!GetCommState(serialPort, &dcbSerialParams))
	{
		return 3;
	}

	dcbSerialParams.BaudRate = baudRate;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;

	if (!SetCommState(serialPort, &dcbSerialParams))
	{
		return 4;
	}

	return 0;
}

int Serial::initCOM(int baudRt)
{
	baudRate = baudRt;

	serialPort = ::CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (serialPort == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			return 1;
		}
		return 2;
	}

	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	if (!GetCommState(serialPort, &dcbSerialParams))
	{
		return 3;
	}

	dcbSerialParams.BaudRate = baudRate;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;

	if (!SetCommState(serialPort, &dcbSerialParams))
	{
		return 4;
	}

	return 0;
}

char Serial::cReadCOM()
{
	DWORD iSize;
	char sReceivedChar;

	ReadFile(serialPort, &sReceivedChar, 1, &iSize, 0);

	if (iSize > 0)
		return sReceivedChar;
	else
		return (-1);
}

void Serial::fReadCOM(std::ofstream *outptf)
{
	DWORD iSize;
	char sReceivedChar;

	ReadFile(serialPort, &sReceivedChar, 1, &iSize, 0);

	if (iSize > 0)										
	{
		*outptf << sReceivedChar;
	}
}


int Serial::cSend(unsigned char *data)
{
	DWORD dwSize = sizeof(*data);   
	DWORD dwBytesWritten;

	BOOL iRet = WriteFile(serialPort, data, dwSize, &dwBytesWritten, NULL);

	return dwBytesWritten;
}

int Serial::cSend(unsigned char *data, int len)
{
	DWORD dwSize = len;
	DWORD dwBytesWritten;

	BOOL iRet = WriteFile(serialPort, data, dwSize, &dwBytesWritten, NULL);

	return dwBytesWritten;
}
int Serial::cSend(char *data)
{
	DWORD dwSize = sizeof(*data);
	DWORD dwBytesWritten;

	BOOL iRet = WriteFile(serialPort, data, dwSize, &dwBytesWritten, NULL);

	return dwBytesWritten;
}

int Serial::cSend(char *data, int len)
{
	DWORD dwSize = len;
	DWORD dwBytesWritten;

	BOOL iRet = WriteFile(serialPort, data, dwSize, &dwBytesWritten, NULL);

	return dwBytesWritten;
}