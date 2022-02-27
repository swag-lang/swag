#include "pch.h"
#include "Log.h"
#include "Os.h"
#include "CommandLine.h"

Log g_Log;

void Log::setDefaultColor()
{
    OS::consoleSetColor(LogColor::Default);
}

void Log::setColor(LogColor color)
{
    curColor = color;
    OS::consoleSetColor(color);
}

void Log::messageHeaderCentered(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor)
{
    if (g_CommandLine->silent)
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
    if (g_CommandLine->silent)
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
    if (g_CommandLine->silent)
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
    if (g_CommandLine->silent || !g_CommandLine->verbose)
        return;
    lock();
    setColor(LogColor::DarkCyan);
    print(message);
    if (forceEol && message.back() != '\n')
        eol();
    setDefaultColor();
    unlock();
}

void Log::lock()
{
    mutexAccess.lock();
}

void Log::unlock()
{
    mutexAccess.unlock();
}

void Log::print(const char* message)
{
    // Markdown
    if (!countLength && g_CommandLine->errorMarkdown)
    {
        while (*message)
        {
            Utf8 tt;
            while (*message && *message != '`')
                tt += *message++;
            cout << tt.c_str();
            tt.clear();
            if (*message == 0)
                break;

            cout << "`";

            auto oldColor = curColor;

            switch (curColor)
            {
            case LogColor::Red:
                tt += "\x1b[91m";
                break;
            case LogColor::White:
                tt += "\x1b[97m";
                break;
            }
            tt += "\x1b[4m";

            message++;
            Utf8 totoSingle;
            while (*message && *message != '`')
            {
                totoSingle += *message;
                tt += *message++;
            }

            if (*message)
                message++;
            tt += "\x1b[0m";

            if (totoSingle.count <= 1)
                cout << totoSingle.c_str();
            else
                cout << tt.c_str();
            tt.clear();

            setColor(oldColor);
            cout << "`";
            if (*message == 0)
                break;
        }

        return;
    }

    if (countLength)
        length += strlen(message);
    cout << message;
}

void Log::printColor(const char* message, LogColor color)
{
    setColor(color);
    cout << message;
}

void Log::print(const Utf8& message)
{
    print(message.c_str());
}

void Log::eol()
{
    cout << "\n";
}

void Log::setCountLength(bool b)
{
    countLength = b;
    length      = 0;
}