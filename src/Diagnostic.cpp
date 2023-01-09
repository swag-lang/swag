#include "pch.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "TypeInfo.h"
#include "ErrorIds.h"

void Diagnostic::setupColors(bool verboseMode)
{
    verboseColor     = LogColor::DarkCyan;
    errorColor       = verboseMode ? verboseColor : LogColor::Red;
    codeColor        = verboseMode ? verboseColor : LogColor::Gray;
    marginCodeColor  = verboseMode ? verboseColor : LogColor::DarkCyan;
    hintColor        = verboseMode ? verboseColor : LogColor::White;
    hilightCodeColor = verboseMode ? verboseColor : LogColor::DarkYellow;
    rangeNoteColor   = verboseMode ? verboseColor : LogColor::White;
    warningColor     = verboseMode ? verboseColor : LogColor::Magenta;
    noteColor        = verboseMode ? verboseColor : LogColor::White;
    stackColor       = verboseMode ? verboseColor : LogColor::DarkYellow;
    remarkColor      = verboseMode ? verboseColor : LogColor::White;
    sourceFileColor  = verboseMode ? verboseColor : LogColor::DarkCyan;
}

void Diagnostic::setup()
{
    if (!hasFile || sourceFile->path.empty())
        showFileName = false;
    if (!hasFile || sourceFile->path.empty() || !hasLocation)
        showSource = false;

    switch (errorLevel)
    {
    case DiagnosticLevel::CallStack:
    case DiagnosticLevel::CallStackInlined:
    case DiagnosticLevel::TraceError:
        showRange             = false;
        showMultipleCodeLines = false;
        break;

    case DiagnosticLevel::Note:
    case DiagnosticLevel::Help:
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

void Diagnostic::printSourceLine()
{
    SWAG_ASSERT(sourceFile);
    auto checkFile = sourceFile;
    if (checkFile->fileForSourceLocation)
        checkFile = checkFile->fileForSourceLocation;

    // Make it relatif to current path (should be shorter) if possible
    fs::path path = checkFile->path;
    if (!g_CommandLine.errorAbsolute)
    {
        error_code errorCode;
        auto       path1 = fs::relative(path, fs::current_path(), errorCode);
        if (!path1.empty())
            path = path1;
    }

    if (!g_CommandLine.errorCompact)
        g_Log.setColor(sourceFileColor);
    g_Log.print(Utf8::normalizePath(path).c_str());
    if (hasRangeLocation)
        g_Log.print(Fmt(":%d:%d:%d:%d: ", startLocation.line + 1, startLocation.column + 1, endLocation.line + 1, endLocation.column + 1));
    else if (hasLocation)
        g_Log.print(Fmt(":%d:%d:%d:%d: ", startLocation.line + 1, startLocation.column + 1, startLocation.line + 1, startLocation.column + 1));
    else
        g_Log.print(": ");
}

void Diagnostic::printMargin(bool eol, bool printLineNo, int lineNo)
{
    if (!printLineNo)
    {
        if (eol)
            g_Log.eol();
        return;
    }

    g_Log.setColor(marginCodeColor);

    if (printLineNo)
    {
        auto l = lineNo;
        int  m = 0;
        while (l)
        {
            l /= 10;
            m++;
        }

        while (m++ < MAX_LINE_DIGITS)
            g_Log.print(" ");
        if (lineNo)
            g_Log.print(Fmt("%d", lineNo));
    }

    g_Log.print(" |  ");
    if (eol)
        g_Log.eol();
}

void Diagnostic::printErrorLevel()
{
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
        g_Log.setColor(warningColor);
        g_Log.print("warning: ");
        break;
    case DiagnosticLevel::Note:
        printMargin(true);
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
        printMargin(true);
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

void Diagnostic::printRemarks()
{
    if (!remarks.empty())
    {
        printMargin(true);

        g_Log.setColor(remarkColor);
        for (auto r : remarks)
        {
            if (r.empty())
                continue;
            printMargin(false);
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

void Diagnostic::collectRanges()
{
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
}

void Diagnostic::reportCompact(bool verboseMode)
{
    setupColors(verboseMode);
    printErrorLevel();
    printSourceLine();
    g_Log.print(textMsg);
    g_Log.eol();
}

void Diagnostic::collectSourceCode()
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
}

void Diagnostic::printSourceCode()
{
    if (!lines.size())
        return;

    if (showRange && (showErrorLevel || showFileName))
        printMargin(true);

    for (int i = 0; i < lines.size() - 1; i++)
    {
        const char* pz = lines[i].c_str();
        if (*pz == 0 || *pz == '\n' || *pz == '\r')
            continue;
        printMargin(false, true, linesNo[i]);
        g_Log.setColor(codeColor);
        g_Log.print(lines[i].c_str() + minBlanks);
        g_Log.eol();
    }

    printMargin(false, true, linesNo.back());

    if (!showRange)
    {
        g_Log.setColor(codeColor);
        g_Log.print(lines.back().c_str() + minBlanks);
        g_Log.eol();
        return;
    }

    auto backLine = lines.back();
    Utf8 errorMsg;
    auto startIndex = minBlanks;
    for (auto& r : ranges)
    {
        // Print before hilighted code
        errorMsg.clear();
        while (startIndex < (int) r.startLocation.column && startIndex < (uint32_t) backLine.length())
        {
            errorMsg += backLine[startIndex];
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

void Diagnostic::setColorRanges(DiagnosticLevel level, bool invertColors)
{
    switch (level)
    {
    case DiagnosticLevel::Error:
        g_Log.setColor(invertColors ? rangeNoteColor : errorColor);
        break;
    case DiagnosticLevel::Warning:
        g_Log.setColor(invertColors ? rangeNoteColor : warningColor);
        break;
    case DiagnosticLevel::Note:
    case DiagnosticLevel::Help:
        g_Log.setColor(invertColors ? errorColor : rangeNoteColor);
        break;
    }
}

void Diagnostic::printRanges()
{
    const auto& backLine = lines.back();
    printMargin(false, true);

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
            setColorRanges(errorLevel, !invertError);
        else
            setColorRanges(errorLevel, invertError);

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
        g_Log.setColor(hintColor);
        g_Log.print(ranges.back().hint);
    }

    if (ranges.size() > 1 && !ranges[0].hint.empty())
    {
        g_Log.eol();
        printMargin(false, true);

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

            setColorRanges(errorLevel, !invertError);
            g_Log.print("|");
        }

        g_Log.eol();
        printMargin(false, true);

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

            g_Log.setColor(hintColor);
            g_Log.print(r.hint);
        }
    }

    g_Log.eol();
}

void Diagnostic::report(bool verboseMode)
{
    setupColors(verboseMode);

    // Message level
    if (showErrorLevel)
    {
        printErrorLevel();
        g_Log.print(textMsg);
        g_Log.eol();
    }
    else
    {
        printMargin(true);
    }

    // Source file and location on their own line
    if (showFileName)
    {
        g_Log.setColor(sourceFileColor);
        g_Log.print("--> ");
        printSourceLine();
        g_Log.eol();
    }

    // Source code
    if (showSource)
    {
        collectSourceCode();
        if (showRange)
            collectRanges();
        printSourceCode();
        if (showRange)
            printRanges();
    }

    // Code remarks
    printRemarks();

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

Utf8 Diagnostic::isType(AstNode* node)
{
    if (!node->typeInfo)
        return "";
    if (node->resolvedSymbolOverload)
        return Fmt(Hnt(Hnt0084), SymTable::getNakedKindName(node->resolvedSymbolOverload).c_str(), node->typeInfo->getDisplayNameC());
    return isType(node->typeInfo);
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
