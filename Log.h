#pragma once
#include "Utf8.h"
#include "CommandLine.h"
#include "Global.h"

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
	Default,
};

enum class LogPassType
{
    PassBegin,
    PassEnd,
    Info,
};

static const int CENTER_COLUMN = 24;

struct Log
{
    void setup();
    void setColor(LogColor color);
    void setDefaultColor();

    void error(const Utf8& message);
    void message(const Utf8& message);
    void messageHeaderCentered(const Utf8& header, const Utf8& message, LogColor headerColor = LogColor::Green, LogColor msgColor = LogColor::White);
    void messageHeaderDot(const Utf8& header, const Utf8& message, LogColor headerColor = LogColor::Cyan, LogColor msgColor = LogColor::Cyan);
    void verbosePass(LogPassType type, const Utf8& message, const Utf8& moduleName, double time = -1);
    void verbose(const Utf8& message, bool forceEol = true);

    void lock()
    {
        mutexAccess.lock();
    }

    void unlock()
    {
        mutexAccess.unlock();
    }

    void print(const char* message)
    {
        cout << message;
    }

    void print(const wchar_t* message)
    {
        cout << message;
    }

    void print(const Utf8& message)
    {
        cout << message.c_str();
    }

    void eol()
    {
        cout << '\n';
    }

    mutex mutexAccess;
};

extern Log g_Log;