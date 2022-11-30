#pragma once
#include "Utf8.h"
#include "Mutex.h"

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

    void message(const Utf8& message);
    void messageHeaderCentered(const Utf8& header, const Utf8& message, LogColor headerColor = LogColor::Green, LogColor msgColor = LogColor::White);
    void messageHeaderDot(const Utf8& header, const Utf8& message, LogColor headerColor = LogColor::Cyan, LogColor msgColor = LogColor::Cyan, const char* dot = ".", bool mustLock = true);
    void verbose(const Utf8& message, bool forceEol = true);

    void lock();
    void unlock();
    void print(const char* message);
    void printColor(const char* message, LogColor color = LogColor::Gray);
    void print(const Utf8& message);
    void eol();
    void setCountLength(bool b);

    Mutex    mutexAccess;
    bool     countLength = false;
    size_t   length      = 0;
    LogColor curColor;
};

extern Log g_Log;