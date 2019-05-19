#pragma once
#include "Utf8.h"

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

class Log
{
    mutex access;
#ifdef WIN32
    HANDLE consoleHandle     = NULL;
    WORD   defaultAttributes = 0;
#endif

public:
    Log();
    void setColor(LogColor color);
    void setDefaultColor();

    void lock()
    {
        access.lock();
    }

    void unlock()
    {
        access.unlock();
    }

    void print(const char* message)
    {
        wcout << message;
    }

    void print(const utf8& message)
    {
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
        wcout << L'\n';
    }
};
