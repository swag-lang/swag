#include "pch.h"
#include "Diagnostic.h"
#include "Log.h"
#include "SourceFile.h"
#include "SymTable.h"
#include "TypeInfo.h"
#include "ErrorIds.h"

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

void Diagnostic::setRange2(const Token& token, const Utf8& h)
{
    hasRangeLocation2 = true;
    startLocation2    = token.startLocation;
    endLocation2      = token.endLocation;
    hint2             = h;
}

void Diagnostic::setRange2(AstNode* node, const Utf8& h)
{
    SourceLocation start, end;
    node->computeLocation(start, end);
    setRange2(start, end, h);
}

bool Diagnostic::mustPrintCode()
{
    return hasFile && !sourceFile->path.empty() && hasLocation && showSource && !g_CommandLine.errorCompact;
}

void Diagnostic::printSourceLine()
{
    if (!hasFile || sourceFile->path.empty() || !showFileName)
        return;

    SWAG_ASSERT(sourceFile);
    auto checkFile = sourceFile;
    if (checkFile->fileForSourceLocation)
        checkFile = checkFile->fileForSourceLocation;

    fs::path path = checkFile->path;

    // Make it relatif to current path (should be shorter) if possible
    if (!g_CommandLine.errorAbsolute)
    {
        error_code errorCode;
        auto       path1 = fs::relative(path, fs::current_path(), errorCode);
        if (!path1.empty())
            path = path1;
    }

    g_Log.print(Utf8::normalizePath(path).c_str());
    if (hasRangeLocation)
        g_Log.print(Fmt(":%d:%d:%d:%d: ", startLocation.line + 1, startLocation.column + 1, endLocation.line + 1, endLocation.column + 1));
    else if (hasLocation)
        g_Log.print(Fmt(":%d:%d:%d:%d: ", startLocation.line + 1, startLocation.column + 1, startLocation.line + 1, startLocation.column + 1));
    else
        g_Log.print(": ");
}

void Diagnostic::printMargin(bool verboseMode, bool eol, int maxDigits, int lineNo)
{
    if (!maxDigits)
    {
        if (eol)
            g_Log.eol();
        return;
    }

    auto verboseColor = LogColor::DarkCyan;
    auto codeColor    = verboseMode ? verboseColor : LogColor::Gray;
    g_Log.setColor(codeColor);

    if (maxDigits)
    {
        auto l = lineNo;
        int  m = 0;
        while (l)
        {
            l /= 10;
            m++;
        }

        while (m++ < maxDigits)
            g_Log.print(" ");
        if (lineNo)
            g_Log.print(Fmt("%d", lineNo));
    }

    g_Log.print(" |  ");
    if (eol)
        g_Log.eol();
}

void Diagnostic::printErrorLevel(bool verboseMode)
{
    auto verboseColor = LogColor::DarkCyan;
    auto errorColor   = verboseMode ? verboseColor : LogColor::Red;
    auto warningColor = verboseMode ? verboseColor : LogColor::Magenta;
    auto noteColor    = verboseMode ? verboseColor : LogColor::Cyan;
    auto stackColor   = verboseMode ? verboseColor : LogColor::DarkYellow;
    g_Log.setColor(verboseMode ? verboseColor : LogColor::White);

    switch (errorLevel)
    {
    case DiagnosticLevel::Error:
        g_Log.setColor(errorColor);
        if (sourceFile && sourceFile->duringSyntax)
            g_Log.print("syntax error: ");
        else
            g_Log.print("error: ");
        break;
    case DiagnosticLevel::Warning:
        if (g_CommandLine.warningsAsErrors)
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
        printMargin(verboseMode, true);
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
        printMargin(verboseMode, true);
        g_Log.setColor(noteColor);
        g_Log.print("help: ");
        break;
    case DiagnosticLevel::CallStack:
    {
        g_Log.setColor(stackColor);
        if (currentStackLevel)
            g_Log.print(Fmt("[callstack:%03u]: ", stackLevel));
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
}

void Diagnostic::printRemarks(bool verboseMode)
{
    if (!remarks.empty())
    {
        printMargin(verboseMode, true);

        auto verboseColor = LogColor::DarkCyan;
        auto remarkColor  = verboseMode ? verboseColor : LogColor::White;
        g_Log.setColor(remarkColor);
        for (auto r : remarks)
        {
            if (r.empty())
                continue;
            printMargin(verboseMode);
            g_Log.setColor(remarkColor);
            g_Log.print(r);
            g_Log.eol();
        }
    }
}

static void fixRange(const Utf8& backLine, SourceLocation& startLocation, int& range, char c1, char c2)
{
    if (range == 1)
        return;

    int decal = startLocation.column;
    int cpt   = 0;
    for (int i = decal; i < backLine.length() && i < decal + range; i++)
    {
        if (backLine[i] == c1)
            cpt++;
        else if (backLine[i] == c2)
            cpt--;
    }

    if (cpt > 0 && decal + range < backLine.length() && backLine[decal + range] == c2)
    {
        range++;
    }
    else if (cpt > 0 && backLine[decal] == c1)
    {
        startLocation.column++;
        range--;
    }
    else if (cpt < 0 && decal && backLine[decal - 1] == c1)
    {
        startLocation.column--;
        range++;
    }
    else if (cpt < 0 && backLine[decal + range - 1] == c2)
    {
        range--;
    }
}

void Diagnostic::reportCompact(bool verboseMode)
{
    printErrorLevel(verboseMode);
    printSourceLine();
    g_Log.print(textMsg);
    g_Log.eol();
}

void Diagnostic::report(bool verboseMode)
{
    auto verboseColor     = LogColor::DarkCyan;
    auto errorColor       = verboseMode ? verboseColor : LogColor::Red;
    auto codeColor        = verboseMode ? verboseColor : LogColor::Gray;
    auto hilightCodeColor = verboseMode ? verboseColor : LogColor::White;
    auto rangeNoteColor   = verboseMode ? verboseColor : LogColor::Cyan;
    g_Log.setColor(verboseMode ? verboseColor : LogColor::White);

    // Message level
    if (showErrorLevel)
    {
        printErrorLevel(verboseMode);
        g_Log.print(textMsg);
        g_Log.eol();
    }
    else
    {
        printMargin(verboseMode, true);
    }

    // Source file and location on their own line
    if (hasFile && !sourceFile->path.empty())
    {
        if (showFileName)
        {
            auto sourceFileColor = verboseMode ? verboseColor : LogColor::Gray;
            g_Log.setColor(sourceFileColor);
            g_Log.print("--> ");
            printSourceLine();
            g_Log.eol();
        }

        if (showErrorLevel || showFileName)
        {
            if (errorLevel == DiagnosticLevel::Note || errorLevel == DiagnosticLevel::Help)
            {
                if (showRange && hasRangeLocation && !showMultipleCodeLines)
                    printMargin(verboseMode, true);
            }
        }
    }

    // Source code
    vector<Utf8> lines;
    vector<int>  linesNo;
    int          maxDigits = 5;

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
                if ((int) location0.line + i < 0)
                    continue;
                bool eof     = false;
                auto oneLine = sourceFile->getLine(location0.line + i, &eof);
                lines.push_back(eof ? Utf8(" ") : oneLine);
                linesNo.push_back(location0.line + i + 1);
            }
        }
        else
        {
            lines.push_back(sourceFile->getLine(location0.line));
            linesNo.push_back(location0.line + 1);
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
            for (auto l : linesNo)
            {
                int m = 0;
                while (l)
                {
                    l /= 10;
                    m++;
                }
                maxDigits = max(maxDigits, m);
            }

            if (showMultipleCodeLines)
                printMargin(verboseMode, true);

            for (int i = 0; i < lines.size(); i++)
            {
                const char* pz = lines[i].c_str();
                if (*pz && *pz != '\n' && *pz != '\r')
                {
                    printMargin(verboseMode, false, maxDigits, linesNo[i]);
                    if (hilightCodeRange && i == lines.size() - 1)
                        break;

                    if (!showRange && errorLevel == DiagnosticLevel::Note)
                    {
                        g_Log.setColor(hilightCodeColor);
                    }
                    else if (errorLevel == DiagnosticLevel::CallStack ||
                             errorLevel == DiagnosticLevel::CallStackInlined ||
                             errorLevel == DiagnosticLevel::TraceError)
                    {
                        g_Log.setColor(hilightCodeColor);
                    }

                    if (lines[i].empty())
                        continue;
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

            bool invertError = false;
            if (hasRangeLocation2 &&
                startLocation2.line == startLocation.line &&
                endLocation2.line == startLocation2.line &&
                endLocation2.column < startLocation.column)
            {
                ranges.push_back({startLocation2, endLocation2, hint2});
                ranges.push_back({startLocation, endLocation, hint});
            }
            else if (hasRangeLocation2 &&
                     startLocation2.line == startLocation.line &&
                     endLocation2.line == startLocation2.line &&
                     startLocation2.column > endLocation.column)
            {
                ranges.push_back({startLocation, endLocation, hint});
                ranges.push_back({startLocation2, endLocation2, hint2});
                invertError = true;
            }
            else
            {
                ranges.push_back({startLocation, endLocation, hint});
            }

            // Preprocess ranges
            for (auto& r : ranges)
            {
                const auto& backLine = lines.back();

                if (r.startLocation.line == r.endLocation.line && r.startLocation.column > r.endLocation.column)
                    swap(r.startLocation.column, r.endLocation.column);

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
                fixRange(backLine, r.startLocation, r.range, '{', '}');
                fixRange(backLine, r.startLocation, r.range, '(', ')');
                fixRange(backLine, r.startLocation, r.range, '[', ']');
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

                        g_Log.setColor(codeColor);
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
                    printMargin(verboseMode, false, maxDigits);

                    auto startIndex = minBlanks;
                    int  rangeIdx   = 0;
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

                        if (rangeIdx != ranges.size() - 1)
                        {
                            switch (errorLevel)
                            {
                            case DiagnosticLevel::Error:
                                g_Log.setColor(invertError ? errorColor : rangeNoteColor);
                                break;
                            case DiagnosticLevel::Note:
                            case DiagnosticLevel::Help:
                                g_Log.setColor(invertError ? rangeNoteColor : errorColor);
                                break;
                            }
                        }
                        else
                        {
                            switch (errorLevel)
                            {
                            case DiagnosticLevel::Error:
                                g_Log.setColor(invertError ? rangeNoteColor : errorColor);
                                break;
                            case DiagnosticLevel::Note:
                            case DiagnosticLevel::Help:
                                g_Log.setColor(invertError ? errorColor : rangeNoteColor);
                                break;
                            }
                        }

                        for (uint32_t i = 0; i < (uint32_t) r.range && i < (uint32_t) backLine.length(); i++)
                        {
                            startIndex++;
                            g_Log.print(r.c);
                        }

                        rangeIdx++;
                    }

                    // Last hint on the same line
                    if (!ranges.back().hint.empty())
                    {
                        g_Log.print(" ");
                        g_Log.setColor(codeColor);
                        g_Log.print(ranges.back().hint);
                    }

                    if (ranges.size() > 1 && !ranges[0].hint.empty())
                    {
                        g_Log.eol();
                        printMargin(verboseMode, false, maxDigits);

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
                                g_Log.setColor(invertError ? errorColor : rangeNoteColor);
                                break;
                            case DiagnosticLevel::Note:
                            case DiagnosticLevel::Help:
                                g_Log.setColor(invertError ? rangeNoteColor : errorColor);
                                break;
                            }

                            g_Log.print("|");
                        }

                        g_Log.eol();
                        printMargin(verboseMode, false, maxDigits);

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

                            g_Log.setColor(codeColor);
                            g_Log.print(r.hint);
                        }
                    }

                    g_Log.eol();
                }
            }
        }
    }

    // Code remarks
    printRemarks(verboseMode);

    g_Log.setDefaultColor();
}

Utf8 Diagnostic::isType(TypeInfo* typeInfo)
{
    if (!typeInfo)
        return "";
    if (typeInfo->isTuple())
        return Hnt(Hnt0010);
    return Fmt(Hnt(Hnt0011), typeInfo->getDisplayNameC());
}

Diagnostic* Diagnostic::hereIs(AstNode* node, bool forceShowRange)
{
    if (node->resolvedSymbolOverload)
        return hereIs(node->resolvedSymbolOverload, forceShowRange);
    auto note       = new Diagnostic{node, node->token, Fmt(Nte(Nte0040), node->token.ctext()), DiagnosticLevel::Note};
    note->showRange = forceShowRange;
    return note;
}

Diagnostic* Diagnostic::hereIs(SymbolOverload* overload, bool forceShowRange)
{
    if (!overload)
        return nullptr;
    if (overload->node && overload->node->flags & AST_GENERATED)
        return nullptr;

    auto refNiceName = SymTable::getArticleKindName(overload);

    auto showRange = false;
    auto site      = overload->node;

    if (site->typeInfo->isTuple())
        showRange = true;

    Diagnostic* note = new Diagnostic{site, site->token, Fmt(Nte(Nte0008), refNiceName.c_str()), DiagnosticLevel::Note};
    if (!forceShowRange)
        note->showRange = showRange;
    return note;
}
