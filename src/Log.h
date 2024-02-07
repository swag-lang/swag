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
    Default,
    Bold,
    UnBold,
    Underline,
    UnUnderline,
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

    Log();
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

    static constexpr LogColor COLOR_COUNT           = LogColor::DarkCyan;
    static constexpr LogColor COLOR_HEADER          = LogColor::DarkCyan;
    static constexpr LogColor COLOR_VALUE           = LogColor::DarkYellow;
    static constexpr LogColor COLOR_BREAKPOINT      = LogColor::Red;
    static constexpr LogColor COLOR_CUR_INSTRUCTION = LogColor::Red;
    static constexpr LogColor COLOR_NAME            = LogColor::DarkYellow;
    static constexpr LogColor COLOR_TYPE            = LogColor::DarkCyan;
    static constexpr LogColor COLOR_LOCATION        = LogColor::DarkMagenta;
    static constexpr LogColor COLOR_INDEX           = LogColor::Cyan;
    static constexpr LogColor COLOR_DEFAULT         = LogColor::Gray;

    Utf8 COLOR_VTS_CUR_INSTRUCTION;
    Utf8 COLOR_VTS_NAME;
    Utf8 COLOR_VTS_TYPE;
    Utf8 COLOR_VTS_LOCATION;
    Utf8 COLOR_VTS_INDEX;
    Utf8 COLOR_VTS_DEFAULT;
    Utf8 COLOR_VTS_BREAKPOINT;

    Mutex        mutexAccess;
    Vector<Utf8> store;
    Utf8         curColor;
    Utf8         storeLine;
    bool         storeMode = false;
};

extern Log g_Log;
