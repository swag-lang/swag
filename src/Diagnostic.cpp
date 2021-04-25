#include "pch.h"
#include "Diagnostic.h"
#include "Log.h"
#include "SourceFile.h"
#include "SymTable.h"

void Diagnostic::defaultColor(bool verboseMode) const
{
    if (verboseMode)
        g_Log.setColor(LogColor::DarkCyan);
    else
        g_Log.setColor(LogColor::White);
}

void Diagnostic::printSourceLine(int headerSize) const
{
    if (g_CommandLine.errorSourceOut)
    {
        g_Log.print("-->");
        for (int i = 0; i < headerSize - 3; i++)
            g_Log.print(" ");
    }

    SWAG_ASSERT(sourceFile);
    fs::path path = sourceFile->path;
    g_Log.print(normalizePath(path).c_str());
    if (hasRangeLocation)
        g_Log.print(format(":%d:%d:%d:%d: ", startLocation.line + 1, startLocation.column + 1, endLocation.line + 1, endLocation.column + 1));
    else if (hasLocation)
        g_Log.print(format(":%d:%d:%d:%d: ", startLocation.line + 1, startLocation.column + 1, startLocation.line + 1, startLocation.column + 1));
    else
        g_Log.print(": ");
}

void Diagnostic::report(bool verboseMode) const
{
    defaultColor(verboseMode);

    // Message level
    switch (errorLevel)
    {
    case DiagnosticLevel::Error:
        if (!verboseMode)
            g_Log.setColor(LogColor::Red);
        g_Log.print("error: ");
        break;
    case DiagnosticLevel::Warning:
        if (g_CommandLine.warningsAsErrors)
        {
            if (!verboseMode)
                g_Log.setColor(LogColor::Red);
            g_Log.print("error: (from warning): ");
        }
        else
        {
            if (!verboseMode)
                g_Log.setColor(LogColor::Magenta);
            g_Log.print("warning: ");
        }
        break;
    case DiagnosticLevel::Note:
        if (!verboseMode)
            g_Log.setColor(LogColor::White);
        g_Log.print("note: ");
        break;
    case DiagnosticLevel::CallStack:
    {
        if (!verboseMode)
            g_Log.setColor(LogColor::DarkYellow);
        if (currentStackLevel)
            g_Log.print(format("callstack:[%03u]: ", stackLevel));
        else
            g_Log.print(format("callstack:%03u: ", stackLevel));
        break;
    }
    case DiagnosticLevel::CallStackInlined:
        if (!verboseMode)
            g_Log.setColor(LogColor::DarkYellow);
        g_Log.print("inlined: ");
        break;
    case DiagnosticLevel::TraceError:
        if (!verboseMode)
            g_Log.setColor(LogColor::DarkYellow);
        g_Log.print("trace error: ");
        break;
    }

    int headerSize = 0;

    // Source line right after the header
    if (!g_CommandLine.errorSourceOut && hasFile && !sourceFile->path.empty())
        printSourceLine(headerSize + 4);

    // User message
    g_Log.print(textMsg);
    g_Log.eol();

    auto remarkColor      = LogColor::White;
    auto sourceFileColor  = LogColor::Cyan;
    auto codeColor        = LogColor::Gray;
    auto hilightCodeColor = LogColor::White;
    auto rangeNoteColor   = LogColor::Cyan;

    // Source file and location on their own line
    if (g_CommandLine.errorSourceOut && hasFile && !sourceFile->path.empty())
    {
        if (!verboseMode)
            g_Log.setColor(sourceFileColor);
        printSourceLine(headerSize + 4);
        g_Log.eol();
    }

    if (!verboseMode)
        g_Log.setColor(codeColor);

    // Source code
    if (hasFile && !sourceFile->path.empty() && hasLocation && printSource && g_CommandLine.errorSourceOut)
    {
        auto location0 = startLocation;
        auto location1 = endLocation;
        location0.line -= sourceFile->getLineOffset;
        location1.line -= sourceFile->getLineOffset;

        // Get all lines of code
        vector<Utf8> lines;
        if (showMultipleCodeLines &&
            errorLevel != DiagnosticLevel::CallStack &&
            errorLevel != DiagnosticLevel::CallStackInlined &&
            errorLevel != DiagnosticLevel::TraceError)
        {
            for (int i = -2; i <= 0; i++)
            {
                if (location0.line + i < 0)
                    continue;
                lines.push_back(sourceFile->getLine(location0.line + i));
            }
        }
        else
        {
            lines.push_back(sourceFile->getLine(location0.line));
        }

        // Remove blanks on the left, but keep indentation
        uint32_t minBlanks = UINT32_MAX;
        for (int i = 0; i < lines.size(); i++)
        {
            auto        line = lines[i];
            const char* pz   = line.c_str();
            if (*pz && *pz != '\n' && *pz != '\r')
            {
                uint32_t countBlanks = 0;
                while (isblank(*pz++))
                    countBlanks++;
                minBlanks = min(minBlanks, countBlanks);
            }
        }

        bool reportRange = showRange &&
                           !lines.empty() &&
                           errorLevel != DiagnosticLevel::CallStack &&
                           errorLevel != DiagnosticLevel::CallStackInlined &&
                           errorLevel != DiagnosticLevel::TraceError;
        auto hilightCodeRange = !verboseMode && reportRange;

        // Print all lines
        if (lines.size())
        {
            for (int j = 0; j < headerSize; j++)
                g_Log.print(" ");
            g_Log.print(" |  \n");

            for (int i = 0; i < lines.size(); i++)
            {
                const char* pz = lines[i].c_str();
                if (*pz && *pz != '\n' && *pz != '\r')
                {
                    for (int j = 0; j < headerSize; j++)
                        g_Log.print(" ");
                    g_Log.print(" |  ");
                    if (hilightCodeRange && i == lines.size() - 1)
                        break;
                    g_Log.print(lines[i].c_str() + minBlanks);
                    g_Log.eol();
                }
            }
        }

        // Show "^^^^^^^"
        if (reportRange)
        {
            for (int lastLine = 0; lastLine < 2; lastLine++)
            {
                if (!hilightCodeRange && lastLine == 0)
                    continue;

                auto startIndex = minBlanks;
                auto backLine   = lines.back();

                int range = 1;
                if (!hasRangeLocation)
                    range = 1;
                else if (location1.line == location0.line)
                    range = endLocation.column - startLocation.column;
                else
                    range = (int) backLine.length() - startLocation.column;
                range = max(1, range);

                // Special case for a range == 1.
                if (range == 1 && (int) startLocation.column < backLine.count)
                {
                    int decal = startLocation.column;

                    // If this is a word, than take the whole word
                    if ((backLine[decal] & 0x80) == 0)
                    {
                        bool isCWord = isalpha(backLine[decal]) || backLine[decal] == '_' || backLine[decal] == '#' || backLine[decal] == '@';
                        if (isCWord)
                        {
                            while (isalnum(backLine[decal + 1]) || backLine[decal + 1] == '_')
                            {
                                decal += 1;
                                range += 1;
                            }
                        }

                        // If this is an operator
#define ISOP(__c) __c == '>' || __c == '<' || __c == '=' || __c == '-' || __c == '+' || __c == '*' || __c == '/' || __c == '%'
                        if (ISOP(backLine[decal]))
                        {
                            while (ISOP(backLine[decal + 1]))
                            {
                                decal += 1;
                                range += 1;
                            }
                        }
                    }
                }

                // Display line with error in color
                if (lastLine == 0)
                {
                    Utf8 errorMsg;
                    while (startIndex < (int) startLocation.column)
                    {
                        if (backLine[startIndex] >= 32)
                            errorMsg += backLine[startIndex];
                        else
                            errorMsg += " ";
                        startIndex++;
                    }

                    g_Log.print(errorMsg);
                    errorMsg.clear();

                    if (startIndex < (uint32_t) backLine.count)
                    {
                        for (int i = 0; i < range; i++)
                        {
                            if (backLine[startIndex] >= 32)
                                errorMsg += backLine[startIndex];
                            else
                                errorMsg += " ";
                            startIndex++;
                        }

                        // Print hilighted code
                        g_Log.setColor(hilightCodeColor);
                        g_Log.print(errorMsg);

                        // Print the remaining part
                        if (startIndex < (uint32_t) backLine.count)
                        {
                            g_Log.setColor(codeColor);
                            g_Log.print(backLine.c_str() + startIndex);
                        }
                    }

                    g_Log.eol();
                }

                // Display markers
                else
                {
                    for (int j = 0; j < headerSize; j++)
                        g_Log.print(" ");
                    g_Log.print(" |  ");

                    for (uint32_t i = minBlanks; i < startLocation.column; i++)
                    {
                        if (backLine[i] == '\t')
                            g_Log.print("\t");
                        else
                            g_Log.print(" ");
                    }

                    if (!verboseMode)
                    {
                        switch (errorLevel)
                        {
                        case DiagnosticLevel::Error:
                            g_Log.setColor(LogColor::Red);
                            break;
                        case DiagnosticLevel::Note:
                            g_Log.setColor(rangeNoteColor);
                            break;
                        }
                    }

                    for (int i = 0; i < range; i++)
                        g_Log.print("^");
                    g_Log.eol();
                }
            }
        }
        else if (lines.size())
        {
            for (int j = 0; j < headerSize; j++)
                g_Log.print(" ");
            g_Log.print(" |  \n");
        }
    }

    // Code remarks
    if (g_CommandLine.errorSourceOut && g_CommandLine.errorNoteOut)
    {
        if (!remarks.empty())
        {
            if (!verboseMode)
                g_Log.setColor(remarkColor);
            for (auto& r : remarks)
            {
                if (r.empty())
                    continue;
                for (int i = 0; i < headerSize; i++)
                    g_Log.print(" ");
                g_Log.print("note: ");
                g_Log.print(r);
                g_Log.eol();
            }
        }
    }

    g_Log.setDefaultColor();
}
