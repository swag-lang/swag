#include "pch.h"
#include "Diagnostic.h"
#include "Log.h"
#include "SourceFile.h"
#include "SymTable.h"
#include "TypeInfo.h"
#include "ErrorIds.h"

Utf8 Hint::isType(TypeInfo* typeInfo)
{
    return Fmt(Hnt(Hnt0011), typeInfo->getDisplayNameC());
}

void Diagnostic::setup()
{
    switch (errorLevel)
    {
    case DiagnosticLevel::CallStack:
    case DiagnosticLevel::CallStackInlined:
    case DiagnosticLevel::Note:
    case DiagnosticLevel::Help:
    case DiagnosticLevel::TraceError:
        showMultipleCodeLines = false;
        break;
    default:
        showMultipleCodeLines = true;
        break;
    }
}

void Diagnostic::setRange2(const SourceLocation& start, const SourceLocation& end, const Utf8& h)
{
    hasRangeLocation2 = true;
    startLocation2    = start;
    endLocation2      = end;
    hint2             = h;
}

void Diagnostic::setRange2(AstNode* node, const Utf8& h)
{
    node->computeEndLocation();
    setRange2(node->token.startLocation, node->token.endLocation, h);
}

bool Diagnostic::mustPrintCode() const
{
    return hasFile && !sourceFile->path.empty() && hasLocation && showSource && g_CommandLine->errorSourceOut;
}

void Diagnostic::printSourceLine() const
{
    SWAG_ASSERT(sourceFile);
    auto checkFile = sourceFile;
    if (checkFile->fileForSourceLocation)
        checkFile = checkFile->fileForSourceLocation;

    fs::path path = checkFile->path;

    // Make it relatif to current path (should be shorter) if possible
    if (!g_CommandLine->errorAbsolute)
    {
        error_code errorCode;
        auto       path1 = fs::relative(path, fs::current_path(), errorCode);
        if (!path1.empty())
            path = path1;
    }

    if (errorLevel != DiagnosticLevel::CallStack && errorLevel != DiagnosticLevel::CallStackInlined)
        g_Log.print("--> ");
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
    static int HEADER_SIZE = 0;
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
    auto sourceFileColor  = verboseMode ? verboseColor : LogColor::Gray;
    auto codeColor        = verboseMode ? verboseColor : LogColor::Gray;
    auto hilightCodeColor = verboseMode ? verboseColor : LogColor::White;
    auto rangeNoteColor   = verboseMode ? verboseColor : LogColor::Cyan;
    auto warningColor     = verboseMode ? verboseColor : LogColor::Magenta;
    auto noteColor        = verboseMode ? verboseColor : LogColor::Cyan;
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
            g_Log.print(Fmt("--> callstack:%03u: ", stackLevel));
        else
            g_Log.print(Fmt("    callstack:%03u: ", stackLevel));
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
    if (!g_CommandLine->errorSourceOut && hasFile && !sourceFile->path.empty() && showFileName)
    {
        printSourceLine();
    }

    // User message
    g_Log.print(textMsg);
    g_Log.eol();

    // Source code
    vector<Utf8> lines;
    g_Log.setColor(codeColor);
    if (mustPrintCode())
    {
        auto location0 = startLocation;
        auto location1 = endLocation;
        location0.line -= sourceFile->getLineOffset;
        location1.line -= sourceFile->getLineOffset;

        // Get all lines of code
        if (showMultipleCodeLines)
        {
            for (int i = -2; i <= 0; i++)
            {
                if (location0.line + i < 0)
                    continue;
                bool eof     = false;
                auto oneLine = sourceFile->getLine(location0.line + i, &eof);
                lines.push_back(eof ? Utf8(" ") : oneLine);
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
                    if (!showRange && errorLevel == DiagnosticLevel::Note)
                        printMargin(hilightCodeColor);
                    else
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
            struct RangeHint
            {
                SourceLocation startLocation;
                SourceLocation endLocation;
                Utf8           hint;
                const char*    c     = "^";
                int            range = 0;
            };
            vector<RangeHint> ranges;

            if (hasRangeLocation2)
                ranges.push_back({startLocation2, endLocation2, hint2, "-"});
            ranges.push_back({startLocation, endLocation, hint});

            // Preprocess ranges
            for (auto& r : ranges)
            {
                const auto& backLine = lines.back();

                r.range = 1;
                if (!hasRangeLocation)
                    r.range = 1;
                else if (r.endLocation.line == r.startLocation.line)
                    r.range = r.endLocation.column - r.startLocation.column;
                else
                    r.range = (int) backLine.length() - r.startLocation.column;
                r.range = max(1, r.range);

                // Special case for a range == 1.
                if (r.range == 1 && (int) r.startLocation.column < backLine.count)
                {
                    int decal = r.startLocation.column;

                    // If this is a word, than take the whole word
                    if ((backLine[decal] & 0x80) == 0)
                    {
                        bool isCWord = isalpha(backLine[decal]) || backLine[decal] == '_' || backLine[decal] == '#' || backLine[decal] == '@';
                        if (isCWord)
                        {
                            while (SWAG_IS_ALNUM(backLine[decal + 1]) || backLine[decal + 1] == '_')
                            {
                                decal += 1;
                                r.range += 1;
                            }
                        }

                        // If this is an operator
#define ISOP(__c) __c == '>' || __c == '<' || __c == '=' || __c == '-' || __c == '+' || __c == '*' || __c == '/' || __c == '%'
                        if (ISOP(backLine[decal]))
                        {
                            while (ISOP(backLine[decal + 1]))
                            {
                                decal += 1;
                                r.range += 1;
                            }
                        }
                    }
                }

                // Fix
                int decal = r.startLocation.column;
                if (backLine[decal] == '{' && backLine[decal + r.range - 1] != '}' && backLine[decal + r.range] == '}')
                    r.range++;
                if (backLine[decal] == '(' && backLine[decal + r.range - 1] != ')' && backLine[decal + r.range] == ')')
                    r.range++;
            }

            for (int lastLine = 0; lastLine < 2; lastLine++)
            {
                if (!hilightCodeRange && lastLine == 0)
                    continue;

                const auto& backLine = lines.back();

                // Display line with error in color
                ///////////////////////////
                if (lastLine == 0)
                {
                    Utf8 errorMsg;
                    auto startIndex = minBlanks;
                    for (auto& r : ranges)
                    {
                        // Print before hilighted code
                        errorMsg.clear();
                        while (startIndex < (int) r.startLocation.column && startIndex < (uint32_t) backLine.length())
                        {
                            if (backLine[startIndex] >= 32 || backLine[startIndex] == '\t')
                                errorMsg += backLine[startIndex];
                            else
                                errorMsg += " ";
                            startIndex++;
                        }

                        g_Log.print(errorMsg);
                        if (startIndex >= (uint32_t) backLine.count)
                            break;

                        // Print hilighted code
                        errorMsg.clear();
                        for (uint32_t i = 0; i < (uint32_t) r.range && i < (uint32_t) backLine.length(); i++)
                        {
                            if (backLine[startIndex] >= 32 || backLine[startIndex] == '\t')
                                errorMsg += backLine[startIndex];
                            startIndex++;
                        }

                        g_Log.setColor(hilightCodeColor);
                        g_Log.print(errorMsg);
                    }

                    // Print the remaining part
                    if (startIndex < (uint32_t) backLine.count)
                    {
                        g_Log.setColor(codeColor);
                        g_Log.print(backLine.c_str() + startIndex);
                    }

                    g_Log.eol();
                }

                // Display markers
                ///////////////////////////
                else
                {
                    printMargin(codeColor, false);

                    auto startIndex = minBlanks;
                    for (auto& r : ranges)
                    {
                        while (startIndex < (int) r.startLocation.column && startIndex < (uint32_t) backLine.length())
                        {
                            if (backLine[startIndex] == '\t')
                                g_Log.print("\t");
                            else
                                g_Log.print(" ");
                            startIndex++;
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

                        for (uint32_t i = 0; i < (uint32_t) r.range && i < (uint32_t) backLine.length(); i++)
                        {
                            startIndex++;
                            g_Log.print(r.c);
                        }
                    }

                    // Last hint on the same line
                    if (!ranges.back().hint.empty())
                    {
                        g_Log.print(" ");
                        g_Log.setColor(remarkColor);
                        g_Log.print(ranges.back().hint);
                    }

                    if (ranges.size() > 1 && !ranges[0].hint.empty())
                    {
                        g_Log.eol();
                        printMargin(codeColor, false);

                        startIndex = minBlanks;
                        for (int ri = 0; ri < ranges.size() - 1; ri++)
                        {
                            const auto& r = ranges[ri];
                            while (startIndex < (int) r.startLocation.column && startIndex < (uint32_t) backLine.length())
                            {
                                if (backLine[startIndex] == '\t')
                                    g_Log.print("\t");
                                else
                                    g_Log.print(" ");
                                startIndex++;
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

                            g_Log.print("|");
                        }

                        g_Log.eol();
                        printMargin(codeColor, false);

                        startIndex = minBlanks;
                        for (int ri = 0; ri < ranges.size() - 1; ri++)
                        {
                            const auto& r = ranges[ri];
                            while (startIndex < (int) r.startLocation.column && startIndex < (uint32_t) backLine.length())
                            {
                                if (backLine[startIndex] == '\t')
                                    g_Log.print("\t");
                                else
                                    g_Log.print(" ");
                                startIndex++;
                            }

                            g_Log.print(r.hint);
                        }
                    }
                }

                g_Log.eol();
                printMargin(codeColor, true);
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
    if (g_CommandLine->errorSourceOut && hasFile && !sourceFile->path.empty() && showFileName)
    {
        printMargin(codeColor);
        g_Log.setColor(sourceFileColor);
        printSourceLine();
        g_Log.eol();
        printMargin(codeColor, true);
    }
    else if (mustPrintCode() && !showRange)
    {
        printMargin(codeColor, true);
    }

    // Code remarks
    if (g_CommandLine->errorSourceOut && g_CommandLine->errorNoteOut)
    {
        if (!remarks.empty())
        {
            if (!mustPrintCode())
                printMargin(codeColor, true);

            g_Log.setColor(remarkColor);
            for (auto r : remarks)
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
}
