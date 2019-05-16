#pragma once
enum class LogColor
{
	Black,
	White,
	Gray,
	Red,
	Blue,
	Green,
	Cyan,
	Yellow,
	Magenta,
	DarkRed,
	DarkBlue,
	DarkGreen,
	DarkCyan,
	DarkYellow,
	DarkMagenta,
};


class Log
{
	mutex access;
#ifdef WIN32
	HANDLE consoleHandle = NULL;
#endif

public:
	Log();

	void lock()							{ access.lock(); }
	void unlock()						{ access.unlock(); }
	void setColor(LogColor color);
	void print(const char *message)		{ printf(message); }
	void print(string message)			{ print(message.c_str()); }
	void print(const wchar_t *message)	{ wprintf(message); }
	void print(wstring message)			{ print(message.c_str()); }
	void eol()							{ print(L"\n"); }
};

