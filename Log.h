#pragma once
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
    HANDLE consoleHandle = NULL;
    WORD   defaultAttributes = 0;
#endif

public:
    Log();

    void lock()
    {
        access.lock();
    }

    void unlock()
    {
        access.unlock();
    }

    void setColor(LogColor color);
    void setDefaultColor();

    void print(const wstring& message)
    {
        wcout << message;
    }

    void eol()
    {
        wcout << L'\n';
    }
};
