#pragma once
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
    Bold,
    UnBold,
};

enum class LogSymbol
{
    VerticalLine,
    HorizontalLine,
    HorizontalLine2,
    HorizontalLineMidVert,
    HorizontalLine2MidVert,
    DownRight,
    DownLeft,
    UpRight,
    DotCenter,
    DotList,
};

enum class LogPassType
{
    PassBegin,
    PassEnd,
    Info,
    Title,
};

struct Log
{
    static constexpr const char* colorToVTS(LogColor color)
    {
        switch (color)
        {
        case LogColor::Bold:
            return "\x1b[1m";
        case LogColor::UnBold:
            return "\x1b[0m";

        case LogColor::Black:
            return "\x1b[30m";

        case LogColor::DarkRed:
            return "\x1b[31m";
        case LogColor::DarkGreen:
            return "\x1b[32m";
        case LogColor::DarkYellow:
            return "\x1b[33m";
        case LogColor::DarkBlue:
            return "\x1b[34m";
        case LogColor::DarkMagenta:
            return "\x1b[35m";
        case LogColor::DarkCyan:
            return "\x1b[36m";
        case LogColor::Gray:
            return "\x1b[37m";

        case LogColor::Red:
            return "\x1b[91m";
        case LogColor::Green:
            return "\x1b[92m";
        case LogColor::Yellow:
            return "\x1b[93m";
        case LogColor::Blue:
            return "\x1b[94m";
        case LogColor::Magenta:
            return "\x1b[95m";
        case LogColor::Cyan:
            return "\x1b[96m";
        case LogColor::White:
            return "\x1b[97m";

        default:
            break;
        }

        return "";
    }

    void setColor(LogColor color);
    void setDefaultColor();

    void lock();
    void unlock();

    void printHeaderDot(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor, const char* dot);
    void printHeaderCentered(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor);
    void print(const char* message);
    void print(const char* message, LogColor color);
    void print(const Utf8& message);
    void print(LogSymbol symbol);
    void eol();

    void messageHeaderCentered(const Utf8& header, const Utf8& message, LogColor headerColor = LogColor::Green, LogColor msgColor = LogColor::White);
    void messageHeaderDot(const Utf8& header, const Utf8& message, LogColor headerColor = LogColor::Cyan, LogColor msgColor = LogColor::Cyan, const char* dot = ".");
    void messageInfo(const Utf8& message);
    void messageVerbose(const Utf8& message);

    void setStoreMode(bool mode);

    Mutex        mutexAccess;
    Vector<Utf8> store;
    Utf8         storeLine;
    bool         storeMode = false;
};

extern Log g_Log;