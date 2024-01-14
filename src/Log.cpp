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

void Log::print(LogSymbol symbol)
{
    switch (symbol)
    {
    case LogSymbol::VerticalLine:
        if (g_CommandLine.logAscii)
            print("|");
        else
            print("\xe2\x94\x82");
        break;

    case LogSymbol::VerticalLineUp:
        if (g_CommandLine.logAscii)
            print("|");
        else
            print("\xe2\x95\xb5");
        break;

    case LogSymbol::DotCenter:
        if (g_CommandLine.logAscii)
            print(".");
        else
            print("\xc2\xb7");
        break;

    case LogSymbol::DotList:
        if (g_CommandLine.logAscii)
            print("*");
        else
            print("\xE2\x80\xa2");
        break;

    case LogSymbol::HorizontalLine:
        if (g_CommandLine.logAscii)
            print("-");
        else
            print("\xe2\x94\x80");
        break;

    case LogSymbol::HorizontalLine2:
        if (g_CommandLine.logAscii)
            print("=");
        else
            print("\xe2\x95\x90");
        break;

    case LogSymbol::HorizontalLineMidVert:
        if (g_CommandLine.logAscii)
            print("-");
        else
            print("\xe2\x94\xac");
        break;

    case LogSymbol::HorizontalLine2MidVert:
        if (g_CommandLine.logAscii)
            print("=");
        else
            print("\xe2\x95\xa4");
        break;

    case LogSymbol::DownRight:
        if (g_CommandLine.logAscii)
            print("|");
        else
            print("\xe2\x94\x94");
        break;

    case LogSymbol::DownLeft:
        if (g_CommandLine.logAscii)
            print("|");
        else
            print("\xe2\x94\x98");
        break;

    case LogSymbol::UpRight:
        if (g_CommandLine.logAscii)
            print("|");
        else
            print("\xe2\x94\x8c");
        break;

    case LogSymbol::RightDown:
        if (g_CommandLine.logAscii)
            print("|");
        else
            print("\xe2\x94\x90");
        break;
    }
}

void Log::print(const char* message)
{
    if (storeMode)
    {
        storeLine += message;
        if (storeLine.back() == '\n')
            eol();
    }
    else
        cout << message;
}

void Log::eol()
{
    if (storeMode)
    {
        store.push_back(storeLine);
        storeLine.clear();
    }
    else
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

void Log::setStoreMode(bool mode)
{
    storeMode = mode;
    if (mode)
    {
        storeLine.clear();
        store.clear();
    }
}