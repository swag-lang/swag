#pragma once
#include "Utf8.h"
#include "CommandLine.h"
#include "Global.h"
#include "SpinLock.h"

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
        mutexAccess.lock();
    }

    void unlock()
    {
        mutexAccess.unlock();
    }

    void print(const char* message)
    {
        wcout << message;
    }

    void print(const wchar_t* message)
    {
        wcout << message;
    }

    void print(const Utf8& message)
    {
        wcout << utf8ToUnicode(message);
    }

    void eol()
    {
        wcout << L'\n';
    }

    void error(const Utf8& message)
    {
        lock();
        setColor(LogColor::Red);
        print(message);
        if (message.back() != '\n')
            eol();
        setDefaultColor();
        unlock();
    }

    void message(const Utf8& message)
    {
        if (g_CommandLine.silent)
            return;
        lock();
        setColor(LogColor::Gray);
        print(message);
        if (message.back() != '\n')
            eol();
        setDefaultColor();
        unlock();
    }

    void verbose(const Utf8& message)
    {
        if (g_CommandLine.silent || !g_CommandLine.verbose)
            return;
        lock();
        setColor(LogColor::DarkCyan);
        print(message);
        if (message.back() != '\n')
            eol();
        setDefaultColor();
        unlock();
    }

    SpinLock mutexAccess;
#ifdef WIN32
    HANDLE consoleHandle     = NULL;
    WORD   defaultAttributes = 0;
#endif
};

extern Log g_Log;