#include "pch.h"
#include "Diagnostic.h"
#include "Ast.h"
#include "LanguageSpec.h"
#include "Naming.h"
#include "SyntaxColor.h"

const uint32_t MAX_INDENT_BLANKS = 10;
const uint32_t MAX_RIGHT_COLUMN  = 80;

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
            if (r.empty() || r[0] == ' ')
                g_Log.print(" ");
            else
                g_Log.print(LogSymbol::DotList);
            g_Log.print(" ");
            g_Log.print(r);
            g_Log.eol();
        }
    }
}

static void fixRange(const Utf8& lineCode, SourceLocation& startLocation, uint32_t& range, char c1, char c2)
{
    if (range == 1)
        return;

    uint32_t decal = startLocation.column;
    uint32_t cpt   = 0;
    for (uint32_t i = decal; i < lineCode.length() && i < decal + range; i++)
    {
        if (lineCode[i] == c1)
            cpt++;
        else if (lineCode[i] == c2)
            cpt--;
    }

    if (cpt > 0 && ((decal + range) < lineCode.length()) && lineCode[decal + range] == c2)
    {
        range++;
    }
    else if (cpt > 0 && (decal < lineCode.length()) && lineCode[decal] == c1)
    {
        startLocation.column++;
        range--;
    }
    else if (cpt < 0 && decal && ((decal - 1) < lineCode.length()) && lineCode[decal - 1] == c1)
    {
        startLocation.column--;
        range++;
    }
    else if (cpt < 0 && ((decal + range - 1) < lineCode.length()) && lineCode[decal + range - 1] == c2)
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
            r.width = (int) lineCode.length() - r.startLocation.column;
        r.width = max(1, r.width);

        // Special case for a range == 1.
        if (r.width == 1 && r.startLocation.column < lineCode.count)
        {
            int decal = r.startLocation.column;

            // If this is a word, than take the whole word
            if ((lineCode[decal] & 0x80) == 0)
            {
                bool isCWord = isalpha(lineCode[decal]) || lineCode[decal] == '_' || lineCode[decal] == '#' || lineCode[decal] == '@';
                if (isCWord)
                {
                    while (SWAG_IS_ALNUM(lineCode[decal + 1]) || lineCode[decal + 1] == '_')
                    {
                        decal += 1;
                        r.width += 1;
                    }
                }

                // If this is an operator
#define ISOP(__c) __c == '>' || __c == '<' || __c == '=' || __c == '-' || __c == '+' || __c == '*' || __c == '/' || __c == '%'
                if (ISOP(lineCode[decal]))
                {
                    while (ISOP(lineCode[decal + 1]))
                    {
                        decal += 1;
                        r.width += 1;
                    }
                }
            }
        }

        // Fix
        fixRange(lineCode, r.startLocation, r.width, '{', '}');
        fixRange(lineCode, r.startLocation, r.width, '(', ')');
        fixRange(lineCode, r.startLocation, r.width, '[', ']');

        r.mid = r.startLocation.column + r.width / 2;
    }
}

void Diagnostic::reportCompact(bool verboseMode)
{
    setupColors(verboseMode);
    printErrorLevel();
    printSourceLine();
    g_Log.print(oneLiner(textMsg));
    g_Log.eol();
}

void Diagnostic::collectSourceCode()
{
    auto location0 = startLocation;
    auto location1 = endLocation;
    location0.line -= sourceFile->offsetGetLine;
    location1.line -= sourceFile->offsetGetLine;

    lineCode    = sourceFile->getLine(location0.line);
    lineCodeNum = location0.line + 1;
    minBlanks   = 0;

    // Remove blanks on the left, but keep indentation
    const char* pz = lineCode.c_str();
    if (*pz && *pz != '\n' && *pz != '\r')
    {
        uint32_t countBlanks = 0;
        while (SWAG_IS_BLANK(*pz))
        {
            pz++;
            countBlanks++;
        }

        if (countBlanks > MAX_INDENT_BLANKS)
            minBlanks = countBlanks - MAX_INDENT_BLANKS;
    }
}

Utf8 Diagnostic::syntax(const Utf8& line, SyntaxColorContext& cxt)
{
    if (!g_CommandLine.logColors)
        return line;
    if (!g_CommandLine.errorSyntaxColor)
        return line;
    return syntaxColor(line, cxt);
}

void Diagnostic::printSourceCode(bool verboseMode)
{
    if (lineCode.empty())
        return;

    if (showRange && (showErrorLevel || showFileName))
        printMargin(true);

    // Print all lines except the one that really contains the relevant stuff (and ranges)
    SyntaxColorContext cxt;
    cxt.mode = SyntaxColorMode::ForLog;

    const char* pz = lineCode.c_str();
    if (*pz == 0 || *pz == '\n' || *pz == '\r')
        return;
    printMargin(false, true, lineCodeNum);

    Utf8 colored;
    if (verboseMode)
    {
        g_Log.setColor(verboseColor);
        colored = lineCode.c_str() + minBlanks;
    }
    else
    {
        g_Log.setColor(LogColor::White);
        colored = syntax(lineCode.c_str() + minBlanks, cxt);
    }

    g_Log.print(colored);
    g_Log.eol();
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

    printMargin(false, true);

#define ALIGN(__where)                                                        \
    while (startIndex < __where && startIndex < (uint32_t) lineCode.length()) \
    {                                                                         \
        if (lineCode[startIndex] == '\t')                                     \
            g_Log.print("\t");                                                \
        else                                                                  \
            g_Log.print(" ");                                                 \
        startIndex++;                                                         \
    }

    // Print all ranges underlines
    auto startIndex = minBlanks;
    for (size_t i = 0; i < ranges.size(); i++)
    {
        const auto& r = ranges[i];

        ALIGN(r.startLocation.column);
        setColorRanges(r.errorLevel);

        while (startIndex < r.mid && startIndex < (uint32_t) lineCode.length())
        {
            startIndex++;
            g_Log.print(LogSymbol::HorizontalLine);
        }

        if (!r.hint.empty() && i != ranges.size() - 1)
        {
            startIndex++;
            g_Log.print(LogSymbol::HorizontalLineMidVert);
        }
        else if (r.startLocation.column == r.endLocation.column)
        {
            startIndex++;
            g_Log.print(LogSymbol::HorizontalLine);
        }

        while (startIndex < r.startLocation.column + r.width && startIndex < (uint32_t) lineCode.length())
        {
            startIndex++;
            g_Log.print(LogSymbol::HorizontalLine);
        }
    }

    // Remove all ranges with an empty message
    for (size_t i = 0; i < ranges.size(); i++)
    {
        if (ranges[i].hint.empty())
        {
            ranges.erase(ranges.begin() + i);
            i--;
        }
    }

    // The last one in on the same line as the underline
    if (ranges.size())
    {
        setColorRanges(ranges.back().errorLevel);
        g_Log.print(" ");
        g_Log.print(ranges.back().hint);
        ranges.pop_back();
    }

    while (ranges.size())
    {
        g_Log.eol();
        printMargin(false, true);

        startIndex = minBlanks;
        for (size_t i = 0; i < ranges.size(); i++)
        {
            const auto& r = ranges[i];
            setColorRanges(r.errorLevel);

            if (i == ranges.size() - 1 &&
                r.mid + 3 + r.hint.length() > MAX_RIGHT_COLUMN &&
                r.mid - 2 - r.hint.length() > minBlanks)
            {
                ALIGN(r.mid - 2 - r.hint.length());
                g_Log.print(r.hint);
                g_Log.print(" ");
                g_Log.print(LogSymbol::HorizontalLine);
                g_Log.print(LogSymbol::DownLeft);
                ranges.clear();
            }
            else if (i == ranges.size() - 1 &&
                     r.mid + 3 + r.hint.length() > MAX_RIGHT_COLUMN)
            {
                g_Log.print(r.hint);
                ranges.clear();
            }
            else if (i == ranges.size() - 1)
            {
                ALIGN(r.mid);
                g_Log.print(LogSymbol::DownRight);
                g_Log.print(LogSymbol::HorizontalLine);
                g_Log.print(" ");
                g_Log.print(r.hint);
                startIndex += r.hint.length() + 4;
                ranges.erase(ranges.begin() + i);
                i--;
            }
            else
            {
                ALIGN(r.mid);
                g_Log.print(LogSymbol::VerticalLine);
                startIndex++;
            }
        }

        // Print all the vertical bars of remaining ranges
        /* if (!ranges.empty())
        {
            g_Log.eol();
            printMargin(false, true);
            startIndex = minBlanks;
            for (const auto& r : ranges)
            {
                ALIGN(r.mid);
                setColorRanges(r.errorLevel);
                g_Log.print(LogSymbol::VerticalLine);
                startIndex++;
            }
        }*/
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
        str = Nte(Nte1010);
    else
        str = Fmt(Nte(Nte1011), typeInfo->getDisplayNameC());

    if (typeInfo->isAlias())
    {
        auto typeAlias = CastTypeInfo<TypeInfoAlias>(typeInfo, TypeInfoKind::Alias);
        if (typeAlias->rawType)
            str += Fmt(" (aka '%s')", typeAlias->rawType->getConcreteAlias()->getDisplayNameC());
    }

    return str;
}

Utf8 Diagnostic::isType(AstNode* node)
{
    if (!node->typeInfo)
        return "";
    if (node->resolvedSymbolOverload)
        return Fmt(Nte(Nte1084), Naming::kindName(node->resolvedSymbolOverload).c_str(), node->typeInfo->getDisplayNameC());
    return isType(node->typeInfo);
}

Diagnostic* Diagnostic::hereIs(AstNode* node, bool forceShowRange, bool forceNode)
{
    if (!forceNode && node->resolvedSymbolOverload)
        return hereIs(node->resolvedSymbolOverload, forceShowRange);

    if (node->resolvedSymbolOverload)
    {
        auto note       = Diagnostic::note(node, node->getTokenName(), Fmt(Nte(Nte0090), Naming::kindName(node->resolvedSymbolOverload).c_str(), node->token.ctext()));
        note->showRange = forceShowRange;
        return note;
    }

    switch (node->kind)
    {
    case AstNodeKind::EnumDecl:
    {
        auto note       = Diagnostic::note(node, node->token, Fmt(Nte(Nte0090), "declaration of enum", node->token.ctext()));
        note->showRange = forceShowRange;
        return note;
    }
    case AstNodeKind::StructDecl:
    {
        auto structDecl = CastAst<AstStruct>(node, AstNodeKind::StructDecl);
        auto note       = Diagnostic::note(structDecl, structDecl->tokenName, Fmt(Nte(Nte0090), "declaration of struct", node->token.ctext()));
        note->showRange = forceShowRange;
        return note;
    }
    case AstNodeKind::FuncDecl:
    {
        auto fctDecl    = CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        auto note       = Diagnostic::note(fctDecl, fctDecl->tokenName, Fmt(Nte(Nte0090), "declaration of function", node->token.ctext()));
        note->showRange = forceShowRange;
        return note;
    }
    default:
        break;
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

    auto site = overload->node;

    if (site->typeInfo && site->typeInfo->isTuple())
    {
        auto note       = Diagnostic::note(site, site->token, Nte(Nte0030));
        note->showRange = false;
        return note;
    }

    auto note       = Diagnostic::note(site, site->getTokenName(), Fmt(Nte(Nte0090), Naming::kindName(overload).c_str(), overload->symbol->name.c_str()));
    note->showRange = forceShowRange;
    return note;
}

void Diagnostic::tokenizeError(const Utf8& err, Vector<Utf8>& tokens)
{
    Utf8::tokenize(err, '$', tokens, false, true);
}

Utf8 Diagnostic::oneLiner(const Utf8& err)
{
    Vector<Utf8> tokens;
    tokenizeError(err, tokens);

    auto result = tokens[0];
    if (tokens.size() > 1)
    {
        result += ": ";
        result += tokens[1];
    }

    return result;
}
