#include "pch.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "TypeInfo.h"
#include "ErrorIds.h"
#include "Naming.h"
#include "Log.h"
#include "LanguageSpec.h"

void Diagnostic::setupColors(bool verboseMode)
{
    verboseColor    = LogColor::DarkCyan;
    errorColor      = verboseMode ? verboseColor : LogColor::Red;
    codeColor       = verboseMode ? verboseColor : LogColor::White;
    marginCodeColor = verboseMode ? verboseColor : LogColor::Gray;
    hintColor       = verboseMode ? verboseColor : LogColor::White;
    rangeNoteColor  = verboseMode ? verboseColor : LogColor::White;
    warningColor    = verboseMode ? verboseColor : LogColor::Magenta;
    noteColor       = verboseMode ? verboseColor : LogColor::Cyan;
    stackColor      = verboseMode ? verboseColor : LogColor::DarkYellow;
    remarkColor     = verboseMode ? verboseColor : LogColor::White;
    autoRemarkColor = verboseMode ? verboseColor : LogColor::Gray;
    sourceFileColor = verboseMode ? verboseColor : LogColor::Gray;
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

    g_Log.print(" ");
    g_Log.print(LogSymbol::VerticalLine);
    g_Log.print("  ");

    if (eol)
        g_Log.eol();
}

void Diagnostic::printErrorLevel()
{
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
}

void Diagnostic::printRemarks()
{
    if (!remarks.empty())
    {
        printMargin(true, true);

        for (auto r : autoRemarks)
        {
            if (r.empty())
                continue;
            printMargin(false, true, 0);
            g_Log.setColor(autoRemarkColor);
            g_Log.print(r);
            g_Log.eol();
        }

        if (!autoRemarks.empty() && !remarks.empty())
            printMargin(true, true);

        for (auto r : remarks)
        {
            if (r.empty())
                continue;
            printMargin(false, true, 0);
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
    if (!g_CommandLine.errorCodeColors)
        return line;

    const char* pz = line.c_str();
    uint32_t    c, offset;
    Utf8        result;

    pz = Utf8::decodeUtf8(pz, c, offset);
    while (c)
    {
        Utf8 identifier;

        // String
        if (c == '"')
        {
            result += Log::colorToVTS(LogColor::SyntaxString);
            result += c;
            while (*pz && *pz != '"')
            {
                if (*pz == '\\')
                    result += *pz++;
                if (*pz)
                    result += *pz++;
            }

            if (*pz == '"')
                result += *pz++;
            pz = Utf8::decodeUtf8(pz, c, offset);
            result += Log::colorToVTS(codeColor);
            continue;
        }

        // Line comment
        if (c == '/' && pz[0] == '/')
        {
            result += Log::colorToVTS(LogColor::SyntaxComment);
            result += c;
            while (*pz && !SWAG_IS_EOL(*pz))
                result += *pz++;
            c = *pz;
            result += Log::colorToVTS(codeColor);
            continue;
        }

        // Binary literal
        if (c == '0' && (*pz == 'b' || *pz == 'B'))
        {
            result += Log::colorToVTS(LogColor::SyntaxNumber);
            result += c;
            result += *pz++;
            while (*pz && (SWAG_IS_HEX(*pz) || *pz == '_'))
                result += *pz++;
            pz = Utf8::decodeUtf8(pz, c, offset);
            result += Log::colorToVTS(codeColor);
            continue;
        }

        // Hexadecimal literal
        if (c == '0' && (*pz == 'x' || *pz == 'X'))
        {
            result += Log::colorToVTS(LogColor::SyntaxNumber);
            result += c;
            result += *pz++;
            while (*pz && (SWAG_IS_HEX(*pz) || *pz == '_'))
                result += *pz++;
            pz = Utf8::decodeUtf8(pz, c, offset);
            result += Log::colorToVTS(codeColor);
            continue;
        }

        // Number
        if (SWAG_IS_DIGIT(c))
        {
            result += Log::colorToVTS(LogColor::SyntaxNumber);
            result += c;

            while (*pz && (SWAG_IS_DIGIT(*pz) || *pz == '_'))
                result += *pz++;

            if (*pz == '.')
            {
                result += *pz++;
                while (*pz && (SWAG_IS_DIGIT(*pz) || *pz == '_'))
                    result += *pz++;
            }

            if (*pz == 'e' || *pz == 'E')
            {
                result += *pz++;
                if (*pz == '-' || *pz == '+')
                    result += *pz++;
                while (*pz && (SWAG_IS_DIGIT(*pz) || *pz == '_'))
                    result += *pz++;
            }

            pz = Utf8::decodeUtf8(pz, c, offset);
            result += Log::colorToVTS(codeColor);
            continue;
        }

        // Word
        if (SWAG_IS_ALPHA(c) || c == '_' || c == '#' || c == '@')
        {
            identifier += c;
            pz = Utf8::decodeUtf8(pz, c, offset);
            while (SWAG_IS_ALPHA(c) || c == '_' || SWAG_IS_DIGIT(c))
            {
                identifier += c;
                pz = Utf8::decodeUtf8(pz, c, offset);
            }

            auto it = g_LangSpec->keywords.find(identifier);
            if (it)
            {
                switch (*it)
                {
                case TokenId::KwdUsing:
                case TokenId::KwdWith:
                case TokenId::KwdCast:
                case TokenId::KwdAutoCast:
                case TokenId::KwdDeRef:
                case TokenId::KwdRef:
                case TokenId::KwdMoveRef:
                case TokenId::KwdRetVal:
                case TokenId::KwdTry:
                case TokenId::KwdCatch:
                case TokenId::KwdTryCatch:
                case TokenId::KwdAssume:
                case TokenId::KwdThrow:
                case TokenId::KwdDiscard:

                case TokenId::KwdVar:
                case TokenId::KwdLet:
                case TokenId::KwdConst:
                case TokenId::KwdUndefined:

                case TokenId::KwdEnum:
                case TokenId::KwdStruct:
                case TokenId::KwdUnion:
                case TokenId::KwdImpl:
                case TokenId::KwdInterface:
                case TokenId::KwdFunc:
                case TokenId::KwdClosure:
                case TokenId::KwdMethod:
                case TokenId::KwdNamespace:
                case TokenId::KwdAlias:
                case TokenId::KwdAttr:

                case TokenId::KwdTrue:
                case TokenId::KwdFalse:
                case TokenId::KwdNull:

                case TokenId::KwdPublic:
                case TokenId::KwdPrivate:
                    result += Log::colorToVTS(LogColor::SyntaxKeyword);
                    result += identifier;
                    result += Log::colorToVTS(codeColor);
                    break;

                case TokenId::NativeType:
                case TokenId::CompilerType:
                    result += Log::colorToVTS(LogColor::SyntaxType);
                    result += identifier;
                    result += Log::colorToVTS(codeColor);
                    break;

                case TokenId::KwdIf:
                case TokenId::KwdElse:
                case TokenId::KwdElif:
                case TokenId::KwdFor:
                case TokenId::KwdWhile:
                case TokenId::KwdSwitch:
                case TokenId::KwdDefer:
                case TokenId::KwdLoop:
                case TokenId::KwdVisit:
                case TokenId::KwdBreak:
                case TokenId::KwdFallThrough:
                case TokenId::KwdUnreachable:
                case TokenId::KwdReturn:
                case TokenId::KwdCase:
                case TokenId::KwdContinue:
                case TokenId::KwdDefault:
                case TokenId::KwdAnd:
                case TokenId::KwdOr:
                case TokenId::KwdOrElse:
                    result += Log::colorToVTS(LogColor::SyntaxLogic);
                    result += identifier;
                    result += Log::colorToVTS(codeColor);
                    break;

                case TokenId::CompilerFuncCompiler:
                case TokenId::CompilerFuncDrop:
                case TokenId::CompilerFuncInit:
                case TokenId::CompilerFuncMain:
                case TokenId::CompilerFuncPreMain:
                case TokenId::CompilerFuncTest:
                    result += Log::colorToVTS(LogColor::SyntaxCompiler);
                    result += identifier;
                    result += Log::colorToVTS(codeColor);
                    break;

                default:
                    if (identifier[0] == '@')
                    {
                        result += Log::colorToVTS(LogColor::SyntaxIntrinsic);
                        result += identifier;
                        result += Log::colorToVTS(codeColor);
                    }
                    else
                    {
                        result += identifier;
                    }

                    break;
                }
            }
            else
            {
                if (identifier[0] >= 'a' and identifier[0] <= 'z' && (c == '(' || c == '\''))
                {
                    result += Log::colorToVTS(LogColor::SyntaxFunction);
                    result += identifier;
                    result += Log::colorToVTS(codeColor);
                }
                else if (identifier[0] >= 'A' and identifier[0] <= 'Z')
                {
                    result += Log::colorToVTS(LogColor::SyntaxConstant);
                    result += identifier;
                    result += Log::colorToVTS(codeColor);
                }
                else
                {
                    result += identifier;
                }
            }

            continue;
        }

        result += c;
        pz = Utf8::decodeUtf8(pz, c, offset);
    }

    return result;
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
        g_Log.setColor(codeColor);
        Utf8 colored;
        if (verboseMode)
            colored = lines[i].c_str() + minBlanks;
        else
            colored = syntax(lines[i].c_str() + minBlanks);
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
    for (auto& r : ranges)
    {
        while (startIndex < r.startLocation.column && startIndex < (uint32_t) backLine.length())
        {
            if (backLine[startIndex] == '\t')
                g_Log.print("\t");
            else
                g_Log.print(" ");
            startIndex++;
        }

        setColorRanges(r.errorLevel);

        for (uint32_t i = 0; i < (uint32_t) r.width && i < (uint32_t) backLine.length(); i++)
        {
            startIndex++;
            if (r.errorLevel == DiagnosticLevel::Error || r.errorLevel == DiagnosticLevel::Panic || r.errorLevel == DiagnosticLevel::Warning)
                g_Log.print("^");
            else
                g_Log.print(LogSymbol::HorizontalLine);
        }
    }

    // Last hint message on the same line
    if (!ranges.empty() && !ranges.back().hint.empty())
    {
        g_Log.print(" ");
        g_Log.setColor(hintColor);
        g_Log.print(ranges.back().hint);
        ranges.pop_back();
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
        g_Log.eol();
        printMargin(false, true);

        // Print all the vertical bars
        startIndex = minBlanks;
        for (const auto& r : ranges)
        {
            while (startIndex < r.startLocation.column && startIndex < (uint32_t) backLine.length())
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

        g_Log.eol();
        printMargin(false, true);

        // Print the last hint
        startIndex = minBlanks;
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

            if (i == ranges.size() - 1)
            {
                g_Log.setColor(hintColor);
                g_Log.print(r.hint);
            }
            else
            {
                setColorRanges(r.errorLevel);
                g_Log.print(LogSymbol::VerticalLine);
                startIndex++;
            }
        }

        // Remove processed range
        ranges.pop_back();
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
        g_Log.print("-->");
        // g_Log.print("\xe2\x87\xa8");
        g_Log.print(" ");
        printSourceLine();
        g_Log.eol();
    }

    // Source code
    if (showSourceCode)
    {
        printSourceCode(verboseMode);
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

Diagnostic* Diagnostic::hereIs(AstNode* node, bool forceShowRange)
{
    if (node->resolvedSymbolOverload)
        return hereIs(node->resolvedSymbolOverload, forceShowRange);

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

    Utf8 refNiceName = "the ";
    refNiceName += Naming::kindName(overload);

    auto showRange = false;
    auto site      = overload->node;

    if (site->typeInfo->isTuple())
        showRange = true;

    auto note = Diagnostic::note(site, site->token, Fmt(Nte(Nte0008), refNiceName.c_str()));
    if (!forceShowRange)
        note->showRange = showRange;
    return note;
}
