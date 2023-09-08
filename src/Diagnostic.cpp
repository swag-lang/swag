#include "pch.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "TypeInfo.h"
#include "ErrorIds.h"
#include "Naming.h"
#include "Log.h"
#include "LanguageSpec.h"
#include "SyntaxColor.h"

void Diagnostic::setupColors(bool verboseMode)
{
    verboseColor      = LogColor::DarkCyan;
    errorColor        = verboseMode ? verboseColor : LogColor::Red;
    marginBorderColor = verboseMode ? verboseColor : LogColor::Cyan;
    codeLineNoColor   = verboseMode ? verboseColor : LogColor::Gray;
    hintColor         = verboseMode ? verboseColor : LogColor::White;
    rangeNoteColor    = verboseMode ? verboseColor : LogColor::White;
    warningColor      = verboseMode ? verboseColor : LogColor::Magenta;
    noteColor         = verboseMode ? verboseColor : LogColor::Cyan;
    stackColor        = verboseMode ? verboseColor : LogColor::DarkYellow;
    remarkColor       = verboseMode ? verboseColor : LogColor::White;
    autoRemarkColor   = verboseMode ? verboseColor : LogColor::Gray;
    sourceFileColor   = verboseMode ? verboseColor : LogColor::Gray;
}

void Diagnostic::setup()
{
    if (!sourceFile || sourceFile->path.empty())
        showFileName = false;
    if (!sourceFile || sourceFile->path.empty() || !hasLocation)
        showSourceCode = false;

    switch (errorLevel)
    {
    case DiagnosticLevel::Note:
    case DiagnosticLevel::Help:
        showMultipleCodeLines = false;
        break;
    default:
        showMultipleCodeLines = true;
        break;
    }
}

void Diagnostic::addRange(const SourceLocation& start, const SourceLocation& end, const Utf8& h)
{
    for (const auto& r : ranges)
    {
        if (r.startLocation.line == start.line &&
            r.startLocation.column == start.column &&
            r.endLocation.line == end.line &&
            r.endLocation.column == end.column)
        {
            return;
        }
    }

    ranges.push_back({start, end, h, DiagnosticLevel::Note});
}

void Diagnostic::addRange(const Token& token, const Utf8& h)
{
    addRange(token.startLocation, token.endLocation, h);
}

void Diagnostic::addRange(AstNode* node, const Utf8& h)
{
    SourceLocation start, end;
    node->computeLocation(start, end);
    addRange(start, end, h);
}

void Diagnostic::printSourceLine()
{
    SWAG_ASSERT(sourceFile);
    auto checkFile = sourceFile;
    if (checkFile->fileForSourceLocation)
        checkFile = checkFile->fileForSourceLocation;

    Path path = checkFile->path;
    if (!g_CommandLine.errorAbsolute)
        path = path.filename();

    if (!g_CommandLine.errorOneLine)
        g_Log.setColor(sourceFileColor);
    g_Log.print(path.string().c_str());
    if (hasLocation && isNote)
        g_Log.print(Fmt(":%d ", startLocation.line + 1));
    else if (hasLocation)
        g_Log.print(Fmt(":%d:%d:%d:%d: ", startLocation.line + 1, startLocation.column + 1, endLocation.line + 1, endLocation.column + 1));
    else
        g_Log.print(": ");
}

void Diagnostic::printMarginLineNo(int lineNo)
{
    g_Log.setColor(codeLineNoColor);

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
    g_Log.print(" ");
}

void Diagnostic::printMargin(bool eol, bool printLineNo, int lineNo)
{
    if (!printLineNo)
    {
        if (eol)
            g_Log.eol();
        return;
    }

    printMarginLineNo(lineNo);

    g_Log.setColor(marginBorderColor);
    g_Log.print(LogSymbol::VerticalLine);
    g_Log.print("  ");

    if (eol)
        g_Log.eol();
}

bool Diagnostic::hastErrorId(const Utf8& textMsg)
{
    if (textMsg.length() > 9 &&
        textMsg[0] == '[' &&
        SWAG_IS_ALPHA(textMsg[1]) &&
        SWAG_IS_ALPHA(textMsg[2]) &&
        SWAG_IS_ALPHA(textMsg[3]) &&
        SWAG_IS_DIGIT(textMsg[4]) &&
        SWAG_IS_DIGIT(textMsg[5]) &&
        SWAG_IS_DIGIT(textMsg[6]) &&
        SWAG_IS_DIGIT(textMsg[7]) &&
        textMsg[8] == ']' &&
        SWAG_IS_BLANK(textMsg[9]))
    {
        return true;
    }

    return false;
}

void Diagnostic::printErrorLevel()
{
    // Put the error ID right after the error level, instead at the beginning of the message
    Utf8 id;
    if (hastErrorId(textMsg))
    {
        id = Utf8(textMsg.buffer, 9);
        textMsg.remove(0, 10);
    }

    switch (errorLevel)
    {
    case DiagnosticLevel::Error:
        g_Log.setColor(errorColor);
        g_Log.print("error: ");
        break;
    case DiagnosticLevel::Panic:
        g_Log.setColor(errorColor);
        g_Log.print("panic: ");
        break;
    case DiagnosticLevel::Warning:
        g_Log.setColor(warningColor);
        g_Log.print("warning: ");
        break;
    case DiagnosticLevel::Note:
        if (emptyMarginBefore)
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
        if (emptyMarginBefore)
            printMargin(true);
        g_Log.setColor(noteColor);
        g_Log.print("help: ");
        break;
    default:
        break;
    }

    if (!id.empty())
    {
        g_Log.print(id);
        g_Log.print(": ");
    }
}

void Diagnostic::printRemarks()
{
    if (!autoRemarks.empty())
    {
        printMargin(true, true);
        for (auto r : autoRemarks)
        {
            if (r.empty())
                continue;
            printMargin(false, true, 0);
            g_Log.setColor(autoRemarkColor);
            g_Log.print(LogSymbol::DotList);
            g_Log.print(" ");
            g_Log.print(r);
            g_Log.eol();
        }
    }

    if (!remarks.empty())
    {
        printMargin(true, true);
        for (auto r : remarks)
        {
            if (r.empty())
                continue;
            printMargin(false, true, 0);
            g_Log.setColor(remarkColor);
            g_Log.print(LogSymbol::DotList);
            g_Log.print(" ");
            g_Log.print(r);
            g_Log.eol();
        }
    }
}

static void fixRange(const Utf8& backLine, SourceLocation& startLocation, int& range, char c1, char c2)
{
    if (range == 1)
        return;

    uint32_t decal = startLocation.column;
    uint32_t cpt   = 0;
    for (uint32_t i = decal; i < backLine.length() && i < decal + range; i++)
    {
        if (backLine[i] == c1)
            cpt++;
        else if (backLine[i] == c2)
            cpt--;
    }

    if (cpt > 0 && ((decal + range) < backLine.length()) && backLine[decal + range] == c2)
    {
        range++;
    }
    else if (cpt > 0 && (decal < backLine.length()) && backLine[decal] == c1)
    {
        startLocation.column++;
        range--;
    }
    else if (cpt < 0 && decal && ((decal - 1) < backLine.length()) && backLine[decal - 1] == c1)
    {
        startLocation.column--;
        range++;
    }
    else if (cpt < 0 && ((decal + range - 1) < backLine.length()) && backLine[decal + range - 1] == c2)
    {
        range--;
    }
}

void Diagnostic::sortRanges()
{
    sort(ranges.begin(), ranges.end(), [](auto& r1, auto& r2)
         { return r1.startLocation.column < r2.startLocation.column; });
}

void Diagnostic::collectRanges()
{
    if (!showRange || !showSourceCode)
        return;
    if (hasLocation)
        ranges.push_back({startLocation, endLocation, hint, errorLevel});
    if (ranges.empty())
        return;

    collectSourceCode();
    sortRanges();

    // Preprocess ranges
    for (auto& r : ranges)
    {
        const auto& backLine = lines.back();

        // No multiline range... a test, to reduce verbosity
        if (r.endLocation.line > r.startLocation.line)
        {
            r.endLocation.line   = r.startLocation.line;
            r.endLocation.column = UINT32_MAX;
        }

        // Be sure start column is before end column
        if (r.startLocation.line == r.endLocation.line && r.startLocation.column > r.endLocation.column)
            swap(r.startLocation.column, r.endLocation.column);

        r.width = 1;
        if (r.endLocation.line == r.startLocation.line)
            r.width = r.endLocation.column - r.startLocation.column;
        else
            r.width = (int) backLine.length() - r.startLocation.column;
        r.width = max(1, r.width);

        // Special case for a range == 1.
        if (r.width == 1 && r.startLocation.column < backLine.count)
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
                        r.width += 1;
                    }
                }

                // If this is an operator
#define ISOP(__c) __c == '>' || __c == '<' || __c == '=' || __c == '-' || __c == '+' || __c == '*' || __c == '/' || __c == '%'
                if (ISOP(backLine[decal]))
                {
                    while (ISOP(backLine[decal + 1]))
                    {
                        decal += 1;
                        r.width += 1;
                    }
                }
            }
        }

        // Fix
        fixRange(backLine, r.startLocation, r.width, '{', '}');
        fixRange(backLine, r.startLocation, r.width, '(', ')');
        fixRange(backLine, r.startLocation, r.width, '[', ']');
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
    location0.line -= sourceFile->offsetGetLine;
    location1.line -= sourceFile->offsetGetLine;

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
    for (size_t i = 0; i < lines.size(); i++)
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

Utf8 Diagnostic::syntax(const Utf8& line)
{
    if (!g_CommandLine.logColors)
        return line;
    if (!g_CommandLine.errorSyntaxColor)
        return line;
    return syntaxColor(line, SyntaxColorMode::ForLog);
}

void Diagnostic::printSourceCode(bool verboseMode)
{
    if (!lines.size())
        return;

    if (showRange && (showErrorLevel || showFileName))
        printMargin(true);

    // Print all lines except the one that really contains the relevant stuff (and ranges)
    for (size_t i = 0; i < lines.size(); i++)
    {
        const char* pz = lines[i].c_str();
        if (*pz == 0 || *pz == '\n' || *pz == '\r')
            continue;
        printMargin(false, true, linesNo[i]);

        Utf8 colored;
        if (verboseMode)
        {
            g_Log.setColor(verboseColor);
            colored = lines[i].c_str() + minBlanks;
        }
        else
        {
            g_Log.setColor(LogColor::White);
            colored = syntax(lines[i].c_str() + minBlanks);
        }

        g_Log.print(colored);
        g_Log.eol();
    }
}

void Diagnostic::setColorRanges(DiagnosticLevel level)
{
    switch (level)
    {
    case DiagnosticLevel::Error:
    case DiagnosticLevel::Panic:
        g_Log.setColor(errorColor);
        break;
    case DiagnosticLevel::Warning:
        g_Log.setColor(warningColor);
        break;
    case DiagnosticLevel::Note:
    case DiagnosticLevel::Help:
        g_Log.setColor(rangeNoteColor);
        break;
    default:
        break;
    }
}

void Diagnostic::printRanges()
{
    if (ranges.empty())
        return;

    const auto& backLine = lines.back();
    printMargin(false, true);

    // Print all marks
    auto startIndex = minBlanks;
    for (size_t i = 0; i < ranges.size(); i++)
    {
        const auto& r = ranges[i];

        while (startIndex < r.startLocation.column && startIndex < (uint32_t) backLine.length())
        {
            if (backLine[startIndex] == '\t')
                g_Log.print("\t");
            else
                g_Log.print(" ");
            startIndex++;
        }

        setColorRanges(r.errorLevel);
        auto mid = r.startLocation.column + r.width / 2;

        while (startIndex < mid && startIndex < (uint32_t) backLine.length())
        {
            startIndex++;
            if (r.errorLevel == DiagnosticLevel::Error || r.errorLevel == DiagnosticLevel::Panic || r.errorLevel == DiagnosticLevel::Warning)
                g_Log.print(LogSymbol::HorizontalLine2);
            else
                g_Log.print(LogSymbol::HorizontalLine);
        }

        if (!r.hint.empty())
        {
            startIndex++;
            if (r.errorLevel == DiagnosticLevel::Error || r.errorLevel == DiagnosticLevel::Panic || r.errorLevel == DiagnosticLevel::Warning)
                g_Log.print(LogSymbol::HorizontalLine2MidVert);
            else
                g_Log.print(LogSymbol::HorizontalLineMidVert);
        }

        while (startIndex < r.startLocation.column + r.width && startIndex < (uint32_t) backLine.length())
        {
            startIndex++;
            if (r.errorLevel == DiagnosticLevel::Error || r.errorLevel == DiagnosticLevel::Panic || r.errorLevel == DiagnosticLevel::Warning)
                g_Log.print(LogSymbol::HorizontalLine2);
            else
                g_Log.print(LogSymbol::HorizontalLine);
        }
    }

    // Remove all ranges with an empty hint
    for (size_t i = 0; i < ranges.size(); i++)
    {
        if (ranges[i].hint.empty())
        {
            ranges.erase(ranges.begin() + i);
            i--;
        }
    }

    while (ranges.size())
    {
        // Print the last hint
        g_Log.eol();
        printMargin(false, true);
        startIndex = minBlanks;
        for (size_t i = 0; i < ranges.size(); i++)
        {
            const auto& r   = ranges[i];
            auto        mid = r.startLocation.column + r.width / 2;

            while (startIndex < mid && startIndex < (uint32_t) backLine.length())
            {
                if (backLine[startIndex] == '\t')
                    g_Log.print("\t");
                else
                    g_Log.print(" ");
                startIndex++;
            }

            if (i == ranges.size() - 1 || startIndex + r.hint.length() + 5 < (ranges[i + 1].startLocation.column + ranges[i + 1].width / 2))
            {
                setColorRanges(r.errorLevel);
                g_Log.print(LogSymbol::DownRight);
                g_Log.print(LogSymbol::HorizontalLine);
                g_Log.print(LogSymbol::HorizontalLine);
                g_Log.print(" ");
                g_Log.setColor(hintColor);
                g_Log.print(r.hint);
                startIndex += r.hint.length() + 4;
                ranges.erase(ranges.begin() + i);
                i--;
            }
            else
            {
                setColorRanges(r.errorLevel);
                g_Log.print(LogSymbol::VerticalLine);
                startIndex++;
            }
        }

        // Print all the vertical bars
        if (!ranges.empty())
        {
            g_Log.eol();
            printMargin(false, true);
            startIndex = minBlanks;
            for (const auto& r : ranges)
            {
                auto mid = r.startLocation.column + r.width / 2;

                while (startIndex < mid && startIndex < (uint32_t) backLine.length())
                {
                    if (backLine[startIndex] == '\t')
                        g_Log.print("\t");
                    else
                        g_Log.print(" ");
                    startIndex++;
                }

                setColorRanges(r.errorLevel);
                g_Log.print(LogSymbol::VerticalLine);
                startIndex++;
            }
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
    else if (!showFileName)
    {
        printMarginLineNo(0);
        g_Log.setColor(marginBorderColor);
        g_Log.print(LogSymbol::VerticalLine);
        g_Log.eol();
    }
    else
    {
        printMargin(true);
    }

    // Source file and location on their own line
    if (showFileName)
    {
        if (showErrorLevel)
            g_Log.eol();
        printMarginLineNo(0);
        g_Log.setColor(marginBorderColor);
        g_Log.print(LogSymbol::UpRight);
        g_Log.print(LogSymbol::HorizontalLine);
        g_Log.print(" ");
        g_Log.setColor(sourceFileColor);
        printSourceLine();
        g_Log.eol();
        printMargin(false, true);
    }

    // Source code
    if (showSourceCode)
    {
        printSourceCode(verboseMode);
        printRanges();
    }

    // Code remarks
    printRemarks();

    if (showSourceCode && closeFileName)
    {
        printMarginLineNo(0);
        g_Log.setColor(marginBorderColor);
        g_Log.print(LogSymbol::DownRight);
        g_Log.print(LogSymbol::HorizontalLine);
        g_Log.eol();
    }

    g_Log.setDefaultColor();
}

Utf8 Diagnostic::isType(TypeInfo* typeInfo)
{
    if (!typeInfo)
        return "";

    Utf8 str;
    if (typeInfo->isTuple() || typeInfo->isListTuple())
        str = Hnt(Hnt0010);
    else
        str = Fmt(Hnt(Hnt0011), typeInfo->getDisplayNameC());

    if (typeInfo->isAlias())
    {
        auto typeAlias = CastTypeInfo<TypeInfoAlias>(typeInfo, TypeInfoKind::Alias);
        if (typeAlias->rawType)
            str += Fmt(" (aka '%s')", typeAlias->rawType->getCA()->getDisplayNameC());
    }

    return str;
}

Utf8 Diagnostic::isType(AstNode* node)
{
    if (!node->typeInfo)
        return "";
    if (node->resolvedSymbolOverload)
        return Fmt(Hnt(Hnt0084), Naming::kindName(node->resolvedSymbolOverload).c_str(), node->typeInfo->getDisplayNameC());
    return isType(node->typeInfo);
}

Diagnostic* Diagnostic::hereIs(AstNode* node, bool forceShowRange, bool forceNode)
{
    if (!forceNode && node->resolvedSymbolOverload)
        return hereIs(node->resolvedSymbolOverload, forceShowRange);

    if (node->resolvedSymbolOverload)
    {
        auto note       = Diagnostic::note(node, node->token, Fmt(Nte(Nte0090), Naming::kindName(node->resolvedSymbolOverload).c_str(), node->token.ctext()));
        note->showRange = forceShowRange;
        return note;
    }

    auto note       = Diagnostic::note(node, node->token, Fmt(Nte(Nte0040), node->token.ctext()));
    note->showRange = forceShowRange;
    return note;
}

Diagnostic* Diagnostic::hereIs(SymbolOverload* overload, bool forceShowRange)
{
    if (!overload)
        return nullptr;
    if (overload->node && overload->node->flags & AST_GENERATED)
        return nullptr;

    auto showRange = false;
    auto site      = overload->node;

    if (site->typeInfo->isTuple())
        showRange = true;

    auto note = Diagnostic::note(site, site->token, Fmt(Nte(Nte0008), Naming::kindName(overload).c_str(), overload->symbol->name.c_str()));
    if (!forceShowRange)
        note->showRange = showRange;
    return note;
}
