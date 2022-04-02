#include "pch.h"
#include "Diagnostic.h"
#include "Log.h"
#include "SourceFile.h"
#include "SymTable.h"
#include "TypeInfo.h"
#include "ErrorIds.h"

thread_local Utf8 g_ErrorHint;
static int        HEADER_SIZE = 0;

Utf8 Hint::isType(TypeInfo* typeInfo)
{
    return Fmt(Hnt(Hnt0011), typeInfo->getDisplayNameC());
}

bool Diagnostic::mustPrintCode() const
{
    return hasFile && !sourceFile->path.empty() && hasLocation && printSource && g_CommandLine->errorSourceOut;
}

void Diagnostic::printSourceLine() const
{
    SWAG_ASSERT(sourceFile);
    auto checkFile = sourceFile;
    if (checkFile->fileForSourceLocation)
        checkFile = checkFile->fileForSourceLocation;

    fs::path path = checkFile->path;
    g_Log.print(Utf8::normalizePath(path).c_str());
    if (hasRangeLocation)
        g_Log.print(Fmt(":%d:%d:%d:%d: ", startLocation.line + 1, startLocation.column + 1, endLocation.line + 1, endLocation.column + 1));
    else if (hasLocation)
        g_Log.print(Fmt(":%d:%d:%d:%d: ", startLocation.line + 1, startLocation.column + 1, startLocation.line + 1, startLocation.column + 1));
    else
        g_Log.print(": ");
}

void Diagnostic::printMargin(LogColor color, bool eol) const
{
    g_Log.setColor(color);
    for (int i = 0; i < HEADER_SIZE; i++)
        g_Log.print(" ");
    g_Log.print(" |  ");
    if (eol)
        g_Log.eol();
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
        if (g_CommandLine->errorSourceOut)
            g_Log.eol();
        g_Log.setColor(errorColor);
        if (sourceFile && sourceFile->duringSyntax)
            g_Log.print("syntax error: ");
        else
            g_Log.print("error: ");
        break;
    case DiagnosticLevel::Warning:
        if (g_CommandLine->errorSourceOut)
            g_Log.eol();
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
        if (noteHeader.empty())
        {
            g_Log.print("note: ");
        }
        else
        {
            g_Log.print(noteHeader);
            g_Log.print(": ");
        }
        break;
    case DiagnosticLevel::Help:
        g_Log.setColor(noteColor);
        g_Log.print("help: ");
        break;
    case DiagnosticLevel::CallStack:
    {
        g_Log.setColor(stackColor);
        if (currentStackLevel)
            g_Log.print(Fmt("callstack:[%03u]: ", stackLevel));
        else
            g_Log.print(Fmt("callstack:%03u: ", stackLevel));
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

    // Source line right after the header
    if (!g_CommandLine->errorSourceOut && hasFile && !sourceFile->path.empty())
    {
        printSourceLine();
    }

    // User message
    g_Log.print(textMsg);
    g_Log.eol();

    // Source code
    g_Log.setColor(codeColor);
    if (mustPrintCode())
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
            errorLevel != DiagnosticLevel::Help &&
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
                while (SWAG_IS_BLANK(*pz))
                {
                    pz++;
                    countBlanks++;
                }

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
            if (errorLevel != DiagnosticLevel::CallStack &&
                errorLevel != DiagnosticLevel::CallStackInlined &&
                errorLevel != DiagnosticLevel::TraceError)
            {
                printMargin(codeColor, true);
            }

            for (int i = 0; i < lines.size(); i++)
            {
                const char* pz = lines[i].c_str();
                if (*pz && *pz != '\n' && *pz != '\r')
                {
                    printMargin(codeColor);
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
                            while (SWAG_IS_ALNUM(backLine[decal + 1]) || backLine[decal + 1] == '_')
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
                    while (startIndex < (int) startLocation.column && startIndex < (uint32_t) backLine.length())
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
                        for (uint32_t i = 0; i < (uint32_t) range && i < (uint32_t) backLine.length(); i++)
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
                    printMargin(codeColor, false);

                    for (uint32_t i = minBlanks; i < startLocation.column && i < (uint32_t) backLine.length(); i++)
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
                    case DiagnosticLevel::Help:
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
                    printMargin(codeColor, true);
                }
            }
        }
        else if (lines.size())
        {
            // Empty line after code
            if (errorLevel != DiagnosticLevel::CallStack &&
                errorLevel != DiagnosticLevel::CallStackInlined &&
                errorLevel != DiagnosticLevel::TraceError &&
                showMultipleCodeLines)
            {
                printMargin(codeColor, true);
            }
        }
    }

    // Source file and location on their own line
    if (g_CommandLine->errorSourceOut && hasFile && !sourceFile->path.empty())
    {
        printMargin(codeColor);
        g_Log.setColor(sourceFileColor);
        printSourceLine();
        g_Log.eol();
        printMargin(codeColor, true);
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
                printMargin(codeColor);
                g_Log.setColor(remarkColor);
                g_Log.print(r);
                g_Log.eol();
            }

            printMargin(codeColor, true);
        }
    }

    g_Log.setDefaultColor();
    g_ErrorHint.clear();
}
