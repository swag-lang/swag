#pragma once
#include "Utf8.h"
#include "CommandLine.h"

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
    Title,
};

static const int CENTER_COLUMN = 24;

struct Log
{
    void setColor(LogColor color);
    void setDefaultColor();

    void error(const Utf8& message);
    void errorOS(const Utf8& message);
    void message(const Utf8& message);
    void messageHeaderCentered(const Utf8& header, const Utf8& message, LogColor headerColor = LogColor::Green, LogColor msgColor = LogColor::White);
    void messageHeaderDot(const Utf8& header, const Utf8& message, LogColor headerColor = LogColor::Cyan, LogColor msgColor = LogColor::Cyan, const char* dot = ".", bool mustLock = true);
    void verbosePass(LogPassType type, const Utf8& message, const Utf8& moduleName, uint64_t time = -1);
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
        if (countLength)
            length += strlen(message);
        cout << message;
    }

    void printColor(const char* message, LogColor color = LogColor::Gray)
    {
        setColor(color);
        cout << message;
    }

    void print(const Utf8& message)
    {
        if (countLength)
            length += message.length();
        cout << message.c_str();
    }

    void eol()
    {
        cout << '\n';
    }

    void setCountLength(bool b)
    {
        countLength = b;
        length      = 0;
    }

    mutex  mutexAccess;
    bool   countLength = false;
    size_t length      = 0;
};

extern Log g_Log;