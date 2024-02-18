#pragma once
#include "Mutex.h"

enum class LogColor
{
    Black,
    White,
    Gray,
    LegitGray,
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
    //
    Bold,
    UnBold,
    Underline,
    UnUnderline,
    //
    Header         = DarkCyan,
    Value          = DarkYellow,
    Breakpoint     = Red,
    CurInstruction = Red,
    Name           = DarkYellow,
    Type           = DarkCyan,
    Location       = DarkMagenta,
    Index          = Cyan,
    Default        = Gray,
};

enum class LogSymbol
{
    VerticalLine,
    VerticalLineDot,
    VerticalLineUp,
    HorizontalLine,
    HorizontalLine2,
    HorizontalLineMidVert,
    HorizontalLine2MidVert,
    DownRight,
    DownLeft,
    UpRight,
    RightDown,
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
    static Utf8 colorToVTS(LogColor color);
    void        setColor(LogColor color);
    void        setDefaultColor();

    void lock();
    void unlock();

    static Utf8 removeFormat(const char* message);
    Utf8        format(const char* message);

    void printHeaderDot(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor, const char* dot);
    void printHeaderCentered(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor);
    void print(const char* message, bool raw = false);
    void print(const char* message, LogColor color);
    void print(const Utf8& message, bool raw = false);
    void print(LogSymbol symbol);
    void eol();

    void messageHeaderCentered(const Utf8& header, const Utf8& message, LogColor headerColor = LogColor::Green, LogColor msgColor = LogColor::White);
    void messageHeaderDot(const Utf8& header, const Utf8& message, LogColor headerColor = LogColor::Cyan, LogColor msgColor = LogColor::Cyan, const char* dot = ".");
    void messageInfo(const Utf8& message);
    void messageVerbose(const Utf8& message);

    void setStoreMode(bool mode);

    Mutex        mutexAccess;
    Vector<Utf8> store;
    Utf8         curColor;
    Utf8         storeLine;
    bool         storeMode = false;
};

extern Log g_Log;
