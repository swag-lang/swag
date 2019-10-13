#include "pch.h"
#include "Log.h"
#ifdef WIN32
#include <fcntl.h>
#include <io.h>

Log g_Log;

void Log::setup()
{
    consoleHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
    _setmode(_fileno(stdout), _O_U16TEXT);

    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(consoleHandle, &info);
    defaultAttributes = info.wAttributes;
}

void Log::setDefaultColor()
{
    ::SetConsoleTextAttribute(consoleHandle, defaultAttributes);
}

void Log::setColor(LogColor color)
{
    WORD attributes{};
    switch (color)
    {
    case LogColor::Black:
        attributes = 0;
        break;
    case LogColor::White:
        attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
    case LogColor::Gray:
        attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
    case LogColor::Red:
        attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;
    case LogColor::Green:
        attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    case LogColor::Blue:
        attributes = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
    case LogColor::Cyan:
        attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    case LogColor::Magenta:
        attributes = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;
    case LogColor::Yellow:
        attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    case LogColor::DarkRed:
        attributes = FOREGROUND_RED;
        break;
    case LogColor::DarkGreen:
        attributes = FOREGROUND_GREEN;
        break;
    case LogColor::DarkBlue:
        attributes = FOREGROUND_BLUE;
        break;
    case LogColor::DarkCyan:
        attributes = FOREGROUND_BLUE | FOREGROUND_GREEN;
        break;
    case LogColor::DarkMagenta:
        attributes = FOREGROUND_BLUE | FOREGROUND_RED;
        break;
    case LogColor::DarkYellow:
        attributes = FOREGROUND_RED | FOREGROUND_GREEN;
        break;
    }

    WORD back = defaultAttributes & (BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY);
    ::SetConsoleTextAttribute(consoleHandle, attributes | back);
}

#endif // WIN32

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