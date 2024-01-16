#include "pch.h"
#include "Diagnostic.h"
#include "Ast.h"
#include "LanguageSpec.h"
#include "Naming.h"
#include "SyntaxColor.h"

void Diagnostic::setupColors()
{
    errorColor        = LogColor::Red;
    marginBorderColor = LogColor::Cyan;
    codeLineNoColor   = LogColor::Cyan;
    hintColor         = LogColor::White;
    rangeNoteColor    = LogColor::White;
    warningColor      = LogColor::Magenta;
    noteColor         = LogColor::White;
    stackColor        = LogColor::DarkYellow;
    preRemarkColor    = LogColor::Cyan;
    remarkColor       = LogColor::Gray;
    autoRemarkColor   = LogColor::Gray;
    sourceFileColor   = LogColor::Cyan;
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
    if (!node)
        return;
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
    if (hasLocation)
        g_Log.print(Fmt(":%d:%d:%d:%d: ", startLocation.line + 1, startLocation.column + 1, endLocation.line + 1, endLocation.column + 1));
    else
        g_Log.print(": ");
}

void Diagnostic::printMarginLineNo(int lineNo)
{
    g_Log.setColor(codeLineNoColor);

    int m = lineNo ? lineCodeNumDigits : 0;
    while (m++ < lineCodeMaxDigits + 1)
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
        g_Log.print("note: ");
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

void Diagnostic::printPreRemarks()
{
    if (!preRemarks.empty())
    {
        for (auto r : preRemarks)
        {
            if (r.empty())
                continue;
            printMargin(false, true, 0);
            if (r.empty() || r[0] == ' ')
            {
                g_Log.setColor(remarkColor);
                g_Log.print(" ");
            }
            else
            {
                g_Log.setColor(preRemarkColor);
                g_Log.print(LogSymbol::DotList);
            }

            g_Log.print(" ");
            g_Log.print(r);
            g_Log.eol();
        }
    }
}

void Diagnostic::printRemarks()
{
    if (!autoRemarks.empty())
    {
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

static void fixRange(const Utf8& lineCode, SourceLocation& startLocation, int& range, char c1, char c2)
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
    if (!showSourceCode)
        return;

    // The main hint is transformed to a range
    if (hasLocation)
    {
        ranges.push_back({startLocation, endLocation, hint, errorLevel});
        hint.clear();
    }

    if (ranges.empty())
        return;

    collectSourceCode();
    sortRanges();

    lineCodeNumDigits = 0;
    auto l            = lineCodeNum;
    while (l)
    {
        l /= 10;
        lineCodeNumDigits++;
    }

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

void Diagnostic::reportCompact()
{
    setupColors();
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

void Diagnostic::printSourceCode()
{
    if (lineCode.empty())
        return;

    if (showErrorLevel || showFileName)
        printMargin(true);

    // Print all lines except the one that really contains the relevant stuff (and ranges)
    SyntaxColorContext cxt;
    cxt.mode = SyntaxColorMode::ForLog;

    const char* pz = lineCode.c_str();
    if (*pz == 0 || *pz == '\n' || *pz == '\r')
        return;
    printMargin(false, true, lineCodeNum);

    g_Log.setColor(LogColor::White);

    auto colored = syntaxColor(lineCode.c_str() + minBlanks, cxt);
    g_Log.print(colored, true);
    g_Log.eol();
}

void Diagnostic::setColorRanges(DiagnosticLevel level)
{
    g_Log.setColor(rangeNoteColor);

    /* switch (level)
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
        }*/
}

void Diagnostic::printRanges()
{
    if (ranges.empty())
        return;

    printMargin(false, true);

#define ALIGN(__where)                                                         \
    while (startIndex < (int) __where && startIndex < (int) lineCode.length()) \
    {                                                                          \
        if (lineCode[startIndex] == '\t')                                      \
            g_Log.print("\t");                                                 \
        else                                                                   \
            g_Log.print(" ");                                                  \
        startIndex++;                                                          \
    }

    // Print all underlines
    int startIndex = minBlanks;
    for (size_t i = 0; i < ranges.size(); i++)
    {
        const auto& r = ranges[i];
        setColorRanges(r.errorLevel);

        ALIGN(r.startLocation.column);

        if (i != ranges.size() - 1 && r.mergeNext)
            setColorRanges(ranges[i + 1].errorLevel);

        while (startIndex < (int) r.startLocation.column + r.width && startIndex < (int) lineCode.length())
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
        auto& r = ranges.back();
        if (ranges.size() != 1 || startIndex + 1 + r.hint.length() < MAX_RIGHT_COLUMN)
        {
            setColorRanges(r.errorLevel);
            g_Log.print(" ");
            g_Log.print(r.hint);
            ranges.pop_back();
        }
    }

    auto numRanges = ranges.size();
    while (ranges.size())
    {
        g_Log.eol();
        printMargin(false, true);
        startIndex = minBlanks;

        for (size_t i = 0; i < ranges.size(); i++)
        {
            const auto& r = ranges[i];
            setColorRanges(r.errorLevel);

            if (i != ranges.size() - 1)
            {
                ALIGN(r.mid);
                g_Log.print(LogSymbol::VerticalLine);
                startIndex++;
                continue;
            }

            auto unformat = g_Log.removeFormat(r.hint.c_str());

            if (ranges.size() == 1 &&
                r.mid + 3 + (int) unformat.length() > (int) MAX_RIGHT_COLUMN &&
                r.mid - 2 - (int) unformat.length() > minBlanks)
            {
                ALIGN(r.mid - 2 - (int) unformat.length());
                g_Log.print(r.hint);
                g_Log.print(" ");
                g_Log.print(LogSymbol::HorizontalLine);
                g_Log.print(LogSymbol::DownLeft);
                ranges.clear();
            }
            else if (ranges.size() == 1 &&
                     r.mid + 3 + unformat.length() > (int) MAX_RIGHT_COLUMN)
            {
                if (numRanges == 1)
                {
                    ALIGN(r.mid);
                    g_Log.print(LogSymbol::VerticalLineUp);
                    g_Log.eol();
                    printMargin(false, true);
                    startIndex = minBlanks;
                }

                setColorRanges(r.errorLevel);
                g_Log.print(r.hint);
                ranges.clear();
            }
            else
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
        }
    }

    g_Log.eol();
}

void Diagnostic::report()
{
    setupColors();

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
        g_Log.print(LogSymbol::VerticalLineDot);
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
        g_Log.print(LogSymbol::VerticalLine);
        g_Log.print(" ");
        g_Log.setColor(sourceFileColor);
        printSourceLine();
        g_Log.eol();
        printMargin(false, true);
    }

    // Code pre remarks
    if (!preRemarks.empty())
    {
        printPreRemarks();
        printMargin(true, true);
    }

    // Source code
    if (showSourceCode)
    {
        printSourceCode();
        printRanges();
    }

    // Code remarks
    if (!autoRemarks.empty() || !remarks.empty())
    {
        printMargin(true, true);
        printRemarks();
    }

    g_Log.setDefaultColor();
}

Utf8 Diagnostic::isType(TypeInfo* typeInfo)
{
    if (!typeInfo)
        return "";

    auto str = Fmt(Nte(Nte1011), typeInfo->getDisplayNameC());

    if (typeInfo->isAlias())
    {
        auto typeAlias = CastTypeInfo<TypeInfoAlias>(typeInfo, TypeInfoKind::Alias);
        if (typeAlias->rawType)
            str += Fmt(" (aka [[%s]])", typeAlias->rawType->getConcreteAlias()->getDisplayNameC());
    }

    return str;
}

Utf8 Diagnostic::isType(SymbolOverload* overload)
{
    if (!overload || !overload->typeInfo)
        return "";
    return Fmt(Nte(Nte1084), Naming::kindName(overload).c_str(), overload->typeInfo->getDisplayNameC());
}

Utf8 Diagnostic::isType(AstNode* node)
{
    if (node->resolvedSymbolOverload)
        return isType(node->resolvedSymbolOverload);
    return isType(node->typeInfo);
}

Diagnostic* Diagnostic::hereIs(SymbolOverload* overload)
{
    return hereIs(overload->node);
}

Diagnostic* Diagnostic::hereIs(AstNode* node)
{
    if (!node)
        return nullptr;

    if (node->flags & AST_GENERATED &&
        node->tokenId != TokenId::KwdPrivate &&
        node->tokenId != TokenId::KwdInternal &&
        node->tokenId != TokenId::KwdPublic)
        return nullptr;

    auto msg = Fmt(Nte(Nte0090), Naming::kindName(node).c_str(), node->token.ctext());
    return Diagnostic::note(node, node->getTokenName(), msg);
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
