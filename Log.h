#pragma once
#include "Utf8.h"
#include "CommandLine.h"
#include "Global.h"

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

struct Log
{
    void setup();
    void setColor(LogColor color);
    void setDefaultColor();

    void lock()
    {
        if (g_CommandLine.silent)
            return;
        mutexAccess.lock();
    }

    void unlock()
    {
        if (g_CommandLine.silent)
            return;
        mutexAccess.unlock();
    }

    void print(const char* message)
    {
        if (g_CommandLine.silent)
            return;
        wcout << message;
    }

    void print(const Utf8& message)
    {
        if (g_CommandLine.silent)
            return;
        try
        {
            wstring_convert<codecvt_utf8<wchar_t>, wchar_t> convert;
            wcout << convert.from_bytes(message);
        }
        catch (...)
        {
            wcout << "?";
        }
    }

    void eol()
    {
        if (g_CommandLine.silent)
            return;
        wcout << L'\n';
    }

    mutex mutexAccess;
#ifdef WIN32
    HANDLE consoleHandle     = NULL;
    WORD   defaultAttributes = 0;
#endif
};
