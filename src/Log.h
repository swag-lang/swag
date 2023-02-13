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
    static constexpr const char* VDarkRed     = "\x1b[31m";
    static constexpr const char* VDarkGreen   = "\x1b[32m";
    static constexpr const char* VDarkYellow  = "\x1b[33m";
    static constexpr const char* VDarkBlue    = "\x1b[34m";
    static constexpr const char* VDarkMagenta = "\x1b[35m";
    static constexpr const char* VDarkCyan    = "\x1b[36m";
    static constexpr const char* VDarkWhite   = "\x1b[37m";

    static const char* colorToVTS(LogColor color);
    void               setColor(LogColor color);
    void               setDefaultColor();

    void messageHeaderCentered(const Utf8& header, const Utf8& message, LogColor headerColor = LogColor::Green, LogColor msgColor = LogColor::White);
    void messageHeaderDot(const Utf8& header, const Utf8& message, LogColor headerColor = LogColor::Cyan, LogColor msgColor = LogColor::Cyan, const char* dot = ".", bool mustLock = true);
    void message(const Utf8& message);
    void verbose(const Utf8& message);

    void lock();
    void unlock();
    void print(const char* message);
    void printColor(const char* message, LogColor color = LogColor::Gray);
    void print(const Utf8& message);
    void eol();
    void setCountLength(bool b);

    Mutex  mutexAccess;
    bool   countLength = false;
    size_t length      = 0;
};

extern Log g_Log;