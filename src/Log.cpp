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

void Log::setDefaultColor()
{
    setColor(LogColor::Gray);
}

void Log::setColor(LogColor color)
{
    if (g_CommandLine.logColors)
        print(colorToVTS(color));
}

void Log::print(const char* message)
{
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

void Log::print(const char* message, LogColor color)
{
    setColor(color);
    print(message);
}

const int CENTER_COLUMN = 24;

void Log::printHeaderDot(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor, const char* dot)
{
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

    if (!message.empty())
    {
        print(message);
        if (message.back() != '\n')
            eol();
    }
}

void Log::printHeaderCentered(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor)
{
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
}

void Log::messageHeaderCentered(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor)
{
    if (g_CommandLine.silent)
        return;

    lock();
    printHeaderCentered(header, message, headerColor, msgColor);
    setDefaultColor();
    unlock();
}

void Log::messageHeaderDot(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor, const char* dot)
{
    if (g_CommandLine.silent)
        return;

    lock();
    printHeaderDot(header, message, headerColor, msgColor, dot);
    setDefaultColor();
    unlock();
}

void Log::messageInfo(const Utf8& message)
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

void Log::messageVerbose(const Utf8& message)
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
