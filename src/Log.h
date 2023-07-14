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

    SyntaxComment,
    SyntaxCompiler,
    SyntaxFunction,
    SyntaxConstant,
    SyntaxIntrinsic,
    SyntaxType,
    SyntaxKeyword,
    SyntaxLogic,
    SyntaxNumber,
    SyntaxString,
};

enum class LogSymbol
{
    VerticalLine,
    HorizontalLine,
    HorizontalLine2,
    HorizontalLineMidVert,
    HorizontalLine2MidVert,
    UpRight,
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

        case LogColor::SyntaxComment:
            return "\x1b[38;2;106;153;85m";
        case LogColor::SyntaxCompiler:
            return "\x1b[38;2;255;116;17m";
        case LogColor::SyntaxFunction:
            return "\x1b[38;2;255;116;17m";
        case LogColor::SyntaxConstant:
            return "\x1b[38;2;78;201;176m";
        case LogColor::SyntaxIntrinsic:
            return "\x1b[38;2;220;220;170m";
        case LogColor::SyntaxType:
            return "\x1b[38;2;246;204;134m";
        case LogColor::SyntaxKeyword:
            return "\x1b[38;2;86;156;214m";
        case LogColor::SyntaxLogic:
            return "\x1b[38;2;216;160;223m";
        case LogColor::SyntaxNumber:
            return "\x1b[38;2;181;206;168m";
        case LogColor::SyntaxString:
            return "\x1b[38;2;206;145;120m";

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

    Mutex mutexAccess;
};

extern Log g_Log;