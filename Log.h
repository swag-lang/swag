#pragma once
#include "Utf8.h"
#include "CommandLine.h"
#include "Global.h"
#include "SpinLock.h"

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

static const int CENTER_COLUMN = 20;

struct Log
{
    void setup();
    void setColor(LogColor color);
    void setDefaultColor();

    void error(const Utf8& message);
    void message(const Utf8& message);
    void messageHeaderCentered(const Utf8& header, const Utf8& message, LogColor headerColor = LogColor::Green, LogColor msgColor = LogColor::White);
    void messageHeaderDot(const Utf8& header, const Utf8& message, LogColor headerColor = LogColor::Cyan, LogColor msgColor = LogColor::Cyan);
    void verbose(const Utf8& message);

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
        wcout << message;
    }

    void print(const wchar_t* message)
    {
        wcout << message;
    }

    void print(const Utf8& message)
    {
        wcout << utf8ToUnicode(message);
    }

    void eol()
    {
        wcout << L'\n';
    }

    mutex mutexAccess;
#ifdef WIN32
    HANDLE consoleHandle     = NULL;
    WORD   defaultAttributes = 0;
#endif
};

extern Log g_Log;