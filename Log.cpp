#include "pch.h"
#include "Log.h"
#ifdef WIN32

Log::Log()
{
	consoleHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
}

void Log::setColor(LogColor color)
{
	WORD   attributes{};
	switch (color)
	{
	case LogColor::Black:
		attributes = 0;
		break;
	case LogColor::White:
		attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		break;
	case LogColor::Gray:
		attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
		break;
	case LogColor::Red:
		attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
		break;
	case LogColor::Green:
		attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		break;
	case LogColor::Blue:
		attributes = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		break;
	case LogColor::Cyan:
		attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		break;
	case LogColor::Magenta:
		attributes = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
		break;
	case LogColor::Yellow:
		attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		break;
	case LogColor::DarkRed:
		attributes = FOREGROUND_RED;
		break;
	case LogColor::DarkGreen:
		attributes = FOREGROUND_GREEN;
		break;
	case LogColor::DarkBlue:
		attributes = FOREGROUND_BLUE;
		break;
	case LogColor::DarkCyan:
		attributes = FOREGROUND_BLUE | FOREGROUND_GREEN;
		break;
	case LogColor::DarkMagenta:
		attributes = FOREGROUND_BLUE | FOREGROUND_RED;
		break;
	case LogColor::DarkYellow:
		attributes = FOREGROUND_RED | FOREGROUND_GREEN;
		break;
	}

	::SetConsoleTextAttribute(consoleHandle, attributes);
}

#endif // WIN32
