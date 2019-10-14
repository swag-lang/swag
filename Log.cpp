#include "pch.h"
#include "Log.h"
#include "Os.h"

Log g_Log;

void Log::setup()
{
    OS::consoleSetup();
}

void Log::setDefaultColor()
{
    OS::consoleSetDefaultColor();
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
    auto size = header.size();
    while (size != CENTER_COLUMN)
    {
        print(" ");
        size++;
    }

    print(header);
    print(" ");
    setColor(msgColor);
    print(message);
    if (message.back() != '\n')
        eol();
    setDefaultColor();
    unlock();
}

void Log::messageHeaderDot(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor)
{
    if (g_CommandLine.silent)
        return;
    lock();

    setColor(headerColor);
    print(header);

    auto size = header.size();
    while (size != CENTER_COLUMN)
    {
        print(".");
        size++;
    }

    print(" ");
    print(message);
    setColor(msgColor);
    if (message.back() != '\n')
        eol();
    setDefaultColor();
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