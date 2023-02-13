#include "pch.h"
#include "Log.h"
#include "Os.h"
#include "CommandLine.h"

Log g_Log;

void Log::lock()
{
    mutexAccess.lock();
}

void Log::unlock()
{
    mutexAccess.unlock();
}

const char* Log::colorToVTS(LogColor color)
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
    }

    return "";
}

void Log::setDefaultColor()
{
    setColor(LogColor::Gray);
}

void Log::setColor(LogColor color)
{
    print(colorToVTS(color));
}

void Log::print(const char* message)
{
    if (countLength)
        length += strlen(message);
    cout << message;
}

void Log::eol()
{
    cout << "\n";
}

void Log::print(const Utf8& message)
{
    print(message.c_str());
}

void Log::printColor(const char* message, LogColor color)
{
    setColor(color);
    print(message);
}

void Log::setCountLength(bool b)
{
    countLength = b;
    length      = 0;
}

void Log::messageHeaderCentered(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor)
{
    if (g_CommandLine.silent)
        return;

    lock();
    setColor(headerColor);
    auto size = header.length();
    while (size < CENTER_COLUMN)
    {
        print(" ");
        size++;
    }

    print(header);
    print(" ");
    setColor(msgColor);
    print(message);
    if (!message.length() || message.back() != '\n')
        eol();
    setDefaultColor();
    unlock();
}

void Log::messageHeaderDot(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor, const char* dot, bool mustLock)
{
    if (g_CommandLine.silent)
        return;
    if (mustLock)
        lock();

    setColor(headerColor);
    print(header);

    auto size = header.length();
    while (size < CENTER_COLUMN)
    {
        print(dot);
        size++;
    }

    print(" ");
    setColor(msgColor);
    print(message);
    if (message.back() != '\n')
        eol();
    setDefaultColor();
    if (mustLock)
        unlock();
}

void Log::message(const Utf8& message)
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

void Log::verbose(const Utf8& message)
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
