#include "pch.h"
#include "Log.h"
#include "Os.h"

Log g_Log;

void Log::setDefaultColor()
{
    OS::consoleSetColor(LogColor::Default);
}

void Log::setColor(LogColor color)
{
    OS::consoleSetColor(color);
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
    print(message);
    setColor(msgColor);
    if (message.back() != '\n')
        eol();
    setDefaultColor();
    if (mustLock)
        unlock();
}

void Log::error(const Utf8& message)
{
    lock();
    setColor(LogColor::Red);
    print(message);
    if (message.back() != '\n')
        eol();
    setDefaultColor();
    unlock();
}

void Log::errorOS(const Utf8& message)
{
    auto str = OS::getLastErrorAsString();
    lock();
    setColor(LogColor::Red);
    SWAG_ASSERT(message.back() != '\n');
    print(message);
    if (!str.empty())
    {
        print(" : ");
        print(str);
    }

    eol();
    setDefaultColor();
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

void Log::verbose(const Utf8& message, bool forceEol)
{
    if (g_CommandLine.silent || !g_CommandLine.verbose)
        return;
    lock();
    setColor(LogColor::DarkCyan);
    print(message);
    if (forceEol && message.back() != '\n')
        eol();
    setDefaultColor();
    unlock();
}