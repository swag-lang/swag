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
    static const char* colorToVTS(LogColor color);
    void               setColor(LogColor color);
    void               setDefaultColor();

    void lock();
    void unlock();

    void printHeaderDot(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor, const char* dot);
    void printHeaderCentered(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor);
    void print(const char* message);
    void print(const char* message, LogColor color);
    void print(const Utf8& message);
    void eol();

    void messageHeaderCentered(const Utf8& header, const Utf8& message, LogColor headerColor = LogColor::Green, LogColor msgColor = LogColor::White);
    void messageHeaderDot(const Utf8& header, const Utf8& message, LogColor headerColor = LogColor::Cyan, LogColor msgColor = LogColor::Cyan, const char* dot = ".");
    void messageInfo(const Utf8& message);
    void messageVerbose(const Utf8& message);

    Mutex mutexAccess;
};

extern Log g_Log;