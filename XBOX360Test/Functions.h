#pragma once

enum ConsoleColorCOUT
{
	Black = 0,
	Blue = 1,
	Green = 2,
	Cyan = 3,
	Red = 4,
	Magenta = 5,
	Brown = 6,
	LightGray = 7,
	DarkGray = 8,
	LightBlue = 9,
	LightGreen = 10,
	LightCyan = 11,
	LightRed = 12,
	LightMagenta = 13,
	Yellow = 14,
	White = 15,
};

enum MessageType
{
	Success = 0,
	Error = 1,
	Header = 2,
	EnterData = 3,
	Line = 4,
	GamepadData = 5,
	RecordingStart = 6,
	RecordingStop = 7,
};

void SetColor(ConsoleColorCOUT);

void coutMessage(MessageType, std::string);

void coutMessage(MessageType);

void cls();

void delayAndCls();
