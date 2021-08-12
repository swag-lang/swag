#include "pch.h"
#include "Diagnostic.h"
#include "Log.h"
#include "SourceFile.h"
#include "SymTable.h"

thread_local Utf8 g_ErrorHint;

void Diagnostic::printSourceLine(int headerSize) const
{
    if (g_CommandLine->errorSourceOut)
    {
        g_Log.print("-->");
        for (int i = 0; i < headerSize - 3; i++)
            g_Log.print(" ");
    }

    SWAG_ASSERT(sourceFile);
    fs::path path = sourceFile->path;
    g_Log.print(Utf8::normalizePath(path).c_str());
    if (hasRangeLocation)
        g_Log.print(Utf8::format(":%d:%d:%d:%d: ", startLocation.line + 1, startLocation.column + 1, endLocation.line + 1, endLocation.column + 1));
    else if (hasLocation)
        g_Log.print(Utf8::format(":%d:%d:%d:%d: ", startLocation.line + 1, startLocation.column + 1, startLocation.line + 1, startLocation.column + 1));
    else
        g_Log.print(": ");
}

void Diagnostic::report(bool verboseMode) const
{
    auto verboseColor     = LogColor::DarkCyan;
    auto errorColor       = verboseMode ? verboseColor : LogColor::Red;
    auto remarkColor      = verboseMode ? verboseColor : LogColor::White;
    auto sourceFileColor  = verboseMode ? verboseColor : LogColor::Cyan;
    auto codeColor        = verboseMode ? verboseColor : LogColor::Gray;
    auto hilightCodeColor = verboseMode ? verboseColor : LogColor::White;
    auto rangeNoteColor   = verboseMode ? verboseColor : LogColor::Cyan;
    auto warningColor     = verboseMode ? verboseColor : LogColor::Magenta;
    auto noteColor        = verboseMode ? verboseColor : LogColor::White;
    auto stackColor       = verboseMode ? verboseColor : LogColor::DarkYellow;
    g_Log.setColor(verboseMode ? verboseColor : LogColor::White);

    // Message level
    switch (errorLevel)
    {
    case DiagnosticLevel::Error:
        g_Log.setColor(errorColor);
        g_Log.print("error: ");
        break;
    case DiagnosticLevel::Warning:
        if (g_CommandLine->warningsAsErrors)
        {
            g_Log.setColor(errorColor);
            g_Log.print("error: (from warning): ");
        }
        else
        {
            g_Log.setColor(warningColor);
            g_Log.print("warning: ");
        }
        break;
    case DiagnosticLevel::Note:
        g_Log.setColor(noteColor);
        g_Log.print("note: ");
        break;
    case DiagnosticLevel::CallStack:
    {
        g_Log.setColor(stackColor);
        if (currentStackLevel)
            g_Log.print(Utf8::format("callstack:[%03u]: ", stackLevel));
        else
            g_Log.print(Utf8::format("callstack:%03u: ", stackLevel));
        break;
    }
    case DiagnosticLevel::CallStackInlined:
        g_Log.setColor(stackColor);
        g_Log.print("inlined: ");
        break;
    case DiagnosticLevel::TraceError:
        g_Log.setColor(stackColor);
        g_Log.print("trace error: ");
        break;
    }

    int headerSize = 0;

    // Source line right after the header
    if (!g_CommandLine->errorSourceOut && hasFile && !sourceFile->path.empty())
        printSourceLine(headerSize + 4);

    // User message
    g_Log.print(textMsg);
    g_Log.eol();

    // Source file and location on their own line
    if (g_CommandLine->errorSourceOut && hasFile && !sourceFile->path.empty())
    {
        g_Log.setColor(sourceFileColor);
        printSourceLine(headerSize + 4);
        g_Log.eol();
    }

    g_Log.setColor(codeColor);

    // Source code
    if (hasFile && !sourceFile->path.empty() && hasLocation && printSource && g_CommandLine->errorSourceOut)
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
            errorLevel != DiagnosticLevel::Note &&
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
                        if (backLine[startIndex] >= 32 || backLine[startIndex] == '\t')
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
                            if (backLine[startIndex] >= 32 || backLine[startIndex] == '\t')
                                errorMsg += backLine[startIndex];
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

                        // Error on multiple lines
                        if ((endLocation.line != startLocation.line) && hasRangeLocation)
                            g_Log.print(" ...");
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

                    switch (errorLevel)
                    {
                    case DiagnosticLevel::Error:
                        g_Log.setColor(errorColor);
                        break;
                    case DiagnosticLevel::Note:
                        g_Log.setColor(rangeNoteColor);
                        break;
                    }

                    // Error on multiple lines
                    if ((endLocation.line != startLocation.line) && hasRangeLocation)
                        range += 4; // for the added " ..."

                    for (int i = 0; i < range; i++)
                        g_Log.print("^");

                    if (!hint.empty())
                    {
                        g_Log.print(" ");
                        g_Log.setColor(remarkColor);
                        g_Log.print(hint);
                    }
                    else if (!g_ErrorHint.empty())
                    {
                        g_Log.print(" ");
                        g_Log.setColor(remarkColor);
                        g_Log.print(g_ErrorHint);
                    }

                    g_Log.eol();

                    g_Log.setColor(codeColor);
                    for (int j = 0; j < headerSize; j++)
                        g_Log.print(" ");
                    g_Log.print(" |  \n");
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
    if (g_CommandLine->errorSourceOut && g_CommandLine->errorNoteOut)
    {
        if (!remarks.empty())
        {
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
    g_ErrorHint.clear();
}
