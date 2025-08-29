#include "pch.h"
#include "Report/Log.h"
#include "Main/CommandLine.h"

Log g_Log;

void Log::lock()
{
    mutexAccess.lock();
}

void Log::unlock()
{
    mutexAccess.unlock();
}

Utf8 Log::colorToVTS(int r, int g, int b)
{
    return form("\x1b[38;2;%d;%d;%dm", r, g, b);
}

Utf8 Log::colorToVTS(LogColor color)
{
    if (!g_CommandLine.logColor)
        return "";

    switch (color)
    {
        case LogColor::Bold:
            return "\x1b[1m";
        case LogColor::UnBold:
            return "\x1b[22m";
        case LogColor::Underline:
            return "\x1b[4m";
        case LogColor::UnUnderline:
            return "\x1b[24m";

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
        case LogColor::LegitGray:
            return "\x1b[37m";
        case LogColor::Gray:
            return colorToVTS(0x8F, 0x8F, 0x8F);

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
    setColor(LogColor::LegitGray);
}

void Log::setColor(LogColor color)
{
    print(colorToVTS(color));
}

void Log::print(LogSymbol symbol)
{
    switch (symbol)
    {
        case LogSymbol::VerticalLine:
            if (g_CommandLine.logAscii)
                write("|");
            else
                write("\xe2\x94\x82");
            break;

        case LogSymbol::VerticalLineDot:
            if (g_CommandLine.logAscii)
                write("|");
            else
                write("\xe2\x94\x86");
            break;

        case LogSymbol::VerticalLineUp:
            if (g_CommandLine.logAscii)
                write("|");
            else
                write("\xe2\x95\xb5");
            break;

        case LogSymbol::DotCenter:
            if (g_CommandLine.logAscii)
                write(".");
            else
                write("\xc2\xb7");
            break;

        case LogSymbol::DotList:
            if (g_CommandLine.logAscii)
                write("*");
            else
                write("\xE2\x80\xa2");
            break;

        case LogSymbol::Cross:
            if (g_CommandLine.logAscii)
                write("X");
            else
                write("\xE2\x9C\x96");
            break;

        case LogSymbol::HorizontalLine:
            if (g_CommandLine.logAscii)
                write("-");
            else
                write("\xe2\x94\x80");
            break;

        case LogSymbol::HorizontalLine2:
            if (g_CommandLine.logAscii)
                write("=");
            else
                write("\xe2\x95\x90");
            break;

        case LogSymbol::HorizontalLineMidVert:
            if (g_CommandLine.logAscii)
                write("-");
            else
                write("\xe2\x94\xac");
            break;

        case LogSymbol::HorizontalLine2MidVert:
            if (g_CommandLine.logAscii)
                write("=");
            else
                write("\xe2\x95\xa4");
            break;

        case LogSymbol::DownRight:
            if (g_CommandLine.logAscii)
                write("|");
            else
                write("\xe2\x94\x94");
            break;

        case LogSymbol::DownLeft:
            if (g_CommandLine.logAscii)
                write("|");
            else
                write("\xe2\x94\x98");
            break;

        case LogSymbol::UpRight:
            if (g_CommandLine.logAscii)
                write("|");
            else
                write("\xe2\x94\x8c");
            break;

        case LogSymbol::RightDown:
            if (g_CommandLine.logAscii)
                write("|");
            else
                write("\xe2\x94\x90");
            break;
    }
}

Utf8 Log::removeFormat(const char* message)
{
    Utf8 m;
    auto pz = message;
    while (*pz)
    {
        if (*pz == '\x1b')
        {
            while (*pz != 'm')
                pz++;
            pz++;
        }
        else if (pz[0] == '[' && pz[1] == '[' && (pz == message || pz[-1] != '['))
        {
            pz += 2;
        }
        else if (pz[0] == ']' && pz[1] == ']' && pz[2] != ']')
        {
            pz += 2;
        }
        else
            m += *pz++;
    }

    return m;
}

Utf8 Log::format(const char* message, const LogWriteContext* logContext)
{
    Utf8 m;
    auto pz = message;
    while (*pz)
    {
        if (*pz == '\x1b')
        {
            curColor.clear();
            while (*pz != 'm')
                curColor += *pz++;
            curColor += *pz++;
            m += curColor;
        }
        else if (pz[0] == '[' && pz[1] == '[' && (pz == message || pz[-1] != '['))
        {
            if (!logContext || logContext->canHighlight)
            {
                if (logContext && !logContext->colorHighlight.empty())
                    m += logContext->colorHighlight;
                else if (curColor == colorToVTS(LogColor::White))
                    m += colorToVTS(LogColor::Gray);
                else
                    m += colorToVTS(LogColor::Underline);
            }

            pz += 2;
        }
        else if (pz[0] == ']' && pz[1] == ']' && pz[2] != ']')
        {
            if (!logContext || logContext->canHighlight)
            {
                m += curColor;
                m += colorToVTS(LogColor::UnUnderline);
                m += colorToVTS(LogColor::UnBold);
            }

            pz += 2;
        }
        else
            m += *pz++;
    }

    return m;
}

void Log::writeEol()
{
    if (g_CommandLine.silent)
        return;

    if (storeMode)
    {
        store.push_back(storeLine);
        storeLine.clear();
    }
    else
        std::cout << "\n";
}

void Log::write(const char* message, const LogWriteContext* logContext)
{
    if (g_CommandLine.silent)
        return;

    if (storeMode)
    {
        storeLine += message;
        if (storeLine.back() == '\n')
            writeEol();
        return;
    }

    if (logContext && logContext->raw)
        std::cout << message;
    else
        std::cout << format(message, logContext).cstr();
}

void Log::print(const Utf8& message, const LogWriteContext* logContext)
{
    if (message.empty())
        return;
    write(message.cstr(), logContext);
}

void Log::print(const char* message, LogColor color)
{
    setColor(color);
    write(message);
}

constexpr int CENTER_COLUMN = 24;

void Log::printHeaderDot(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor, const char* dot)
{
    setColor(headerColor);
    print(header);

    auto size = header.length();
    while (size < CENTER_COLUMN)
    {
        write(dot);
        size++;
    }

    write(" ");
    setColor(msgColor);

    if (!message.empty())
    {
        print(message);
        if (message.back() != '\n')
            writeEol();
    }
}

void Log::printHeaderCentered(const Utf8& header, const Utf8& message, LogColor headerColor, LogColor msgColor)
{
    setColor(headerColor);
    auto size = header.length();
    while (size < CENTER_COLUMN)
    {
        write(" ");
        size++;
    }

    print(header);
    write(" ");
    setColor(msgColor);
    print(message);
    if (message.empty() || message.back() != '\n')
        writeEol();
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
        writeEol();
    setDefaultColor();
    unlock();
}

void Log::messageVerbose(const Utf8& message)
{
    if (!g_CommandLine.verbose)
        return;

    lock();
    setColor(LogColor::DarkCyan);
    print(message);
    if (message.back() != '\n')
        writeEol();
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
