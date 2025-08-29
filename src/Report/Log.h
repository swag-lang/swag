#pragma once
#include "Threading/Mutex.h"

struct LogWriteContext
{
    Utf8 colorHighlight;
    bool raw          = false;
    bool canHighlight = true;
};

enum class LogColor
{
    Black       = 0,
    White       = 1,
    Gray        = 2,
    LegitGray   = 3,
    Red         = 4,
    Blue        = 5,
    Green       = 6,
    Cyan        = 7,
    Yellow      = 8,
    Magenta     = 9,
    DarkRed     = 10,
    DarkBlue    = 11,
    DarkGreen   = 12,
    DarkCyan    = 13,
    DarkYellow  = 14,
    DarkMagenta = 15,
    //
    Bold        = 16,
    UnBold      = 17,
    Underline   = 18,
    UnUnderline = 19,
    //
    Header           = DarkCyan,
    Value            = DarkYellow,
    Breakpoint       = Red,
    CurInstruction   = Red,
    Name             = DarkYellow,
    Type             = DarkCyan,
    Location         = DarkMagenta,
    Index            = Cyan,
    Default          = Gray,
    ByteCodeFlags    = Gray,
    SourceCode       = DarkYellow,
    Instruction      = White,
    InstructionFlags = White,
    //
    Undefined   = -1,
    Transparent = -2,
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
    Cross,
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
    static Utf8 colorToVTS(int r, int g, int b);
    static Utf8 colorToVTS(LogColor color);
    void        setColor(LogColor color);
    void        setDefaultColor();

    void lock();
    void unlock();

    static Utf8 removeFormat(const char* message);
    Utf8        format(const char* message, const LogWriteContext* logContext);

    void printHeaderDot(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor, const char* dot);
    void printHeaderCentered(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor);
    void write(const char* message, const LogWriteContext* logContext = nullptr);
    void print(const char* message, LogColor color);
    void print(const Utf8& message, const LogWriteContext* logContext = nullptr);
    void print(LogSymbol symbol);
    void writeEol();

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
