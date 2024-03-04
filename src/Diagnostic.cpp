#include "pch.h"
#include "Diagnostic.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ErrorIds.h"
#include "Log.h"
#include "Naming.h"
#include "SourceFile.h"
#include "Symbol.h"
#include "SyntaxColor.h"
#include "TypeInfo.h"

void Diagnostic::setupColors()
{
    errorColor        = LogColor::Red;
    marginBorderColor = LogColor::Cyan;
    codeLineNoColor   = LogColor::Cyan;
    hintColor         = LogColor::White;
    rangeNoteColor    = LogColor::White;
    warningColor      = LogColor::Magenta;
    noteColor         = LogColor::White;
    noteTitleColor    = LogColor::DarkYellow;
    stackColor        = LogColor::DarkYellow;
    preRemarkColor    = LogColor::White;
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
    hasContent = showSourceCode || !remarks.empty() || !autoRemarks.empty() || !preRemarks.empty();
}

void Diagnostic::addNote(const SourceLocation& start, const SourceLocation& end, const Utf8& h)
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

    const auto note = Diagnostic::note(sourceFile, start, end, h);
    notes.push_back(note);
}

void Diagnostic::addNote(const Token& token, const Utf8& h)
{
    addNote(token.startLocation, token.endLocation, h);
}

void Diagnostic::addNote(AstNode* node, const Token& token, const Utf8& msg)
{
    notes.push_back(note(node, token, msg));
}

void Diagnostic::addNote(const Utf8& msg)
{
    notes.push_back(note(msg));
}

void Diagnostic::addNote(const Diagnostic* note)
{
    if (!note)
        return;
    notes.push_back(note);
}

void Diagnostic::addNote(SourceFile* file, const Token& token, const Utf8& msg)
{
    notes.push_back(note(file, token, msg));
}

void Diagnostic::addNote(SourceFile* file, const SourceLocation& start, const SourceLocation& end, const Utf8& msg)
{
    notes.push_back(note(file, start, end, msg));
}

void Diagnostic::addNote(AstNode* node, const Utf8& h)
{
    if (!node)
        return;

    SourceLocation start, end;
    node->computeLocation(start, end);
    notes.push_back(note(node->token.sourceFile, start, end, h));
}

void Diagnostic::printSourceLine() const
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
    g_Log.print(path);
    if (hasLocation)
        g_Log.print(form(":%d:%d:%d:%d: ", startLocation.line + 1, startLocation.column + 1, endLocation.line + 1, endLocation.column + 1));
    else
        g_Log.write(": ");
}

void Diagnostic::printMarginLineNo(int lineNo) const
{
    g_Log.setColor(codeLineNoColor);

    int m = lineNo ? lineCodeNumDigits : 0;
    while (m++ < lineCodeMaxDigits + 1)
        g_Log.write(" ");
    if (lineNo)
        g_Log.print(form("%d", lineNo));
    g_Log.write(" ");
}

void Diagnostic::printMargin(bool eol, bool printLineNo, int lineNo) const
{
    if (!printLineNo)
    {
        if (eol)
            g_Log.writeEol();
        return;
    }

    printMarginLineNo(lineNo);

    g_Log.setColor(marginBorderColor);
    g_Log.print(LogSymbol::VerticalLine);
    g_Log.write(" ");

    if (eol)
        g_Log.writeEol();
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
            g_Log.write("error: ");
            break;

        case DiagnosticLevel::Panic:
            g_Log.setColor(errorColor);
            g_Log.write("panic: ");
            break;

        case DiagnosticLevel::Warning:
            g_Log.setColor(warningColor);
            g_Log.write("warning: ");
            break;

        case DiagnosticLevel::Note:
            g_Log.setColor(noteTitleColor);
            g_Log.write("note: ");
            g_Log.setColor(noteColor);
            break;
    }

    if (!id.empty())
    {
        g_Log.print(id);
        g_Log.write(": ");
    }
}

void Diagnostic::printPreRemarks() const
{
    if (!preRemarks.empty())
    {
        for (const auto& r : preRemarks)
        {
            if (r.empty())
                continue;
            printMargin(false, true, 0);
            if (r.empty() || r[0] == ' ')
            {
                g_Log.setColor(remarkColor);
                g_Log.write(" ");
            }
            else
            {
                g_Log.setColor(preRemarkColor);
                g_Log.print(LogSymbol::DotList);
            }

            g_Log.write(" ");
            g_Log.print(r);
            g_Log.writeEol();
        }
    }
}

void Diagnostic::printRemarks() const
{
    if (!autoRemarks.empty())
    {
        for (const auto& r : autoRemarks)
        {
            if (r.empty())
                continue;
            printMargin(false, true, 0);
            g_Log.setColor(autoRemarkColor);
            g_Log.print(LogSymbol::DotList);
            g_Log.write(" ");
            g_Log.print(r);
            g_Log.writeEol();
        }
    }

    if (!remarks.empty())
    {
        for (const auto& r : remarks)
        {
            if (r.empty())
                continue;
            printMargin(false, true, 0);
            g_Log.setColor(remarkColor);
            g_Log.print(LogSymbol::DotList);
            g_Log.write(" ");
            g_Log.print(r);
            g_Log.writeEol();
        }
    }
}

namespace
{
    void fixRange(const Utf8& lineCode, SourceLocation& startLocation, int& range, char c1, char c2)
    {
        if (range == 1)
            return;

        const int decal = static_cast<int>(startLocation.column);
        int       cpt   = 0;

        for (int i = decal; i < static_cast<int>(lineCode.length()) && i < decal + range; i++)
        {
            if (lineCode[i] == c1)
                cpt++;
            else if (lineCode[i] == c2)
                cpt--;
        }

        if (cpt > 0 && decal + range < static_cast<int>(lineCode.length()) && lineCode[decal + range] == c2)
        {
            range++;
        }
        else if (cpt > 0 && decal < static_cast<int>(lineCode.length()) && lineCode[decal] == c1)
        {
            startLocation.column++;
            range--;
        }
        else if (cpt < 0 && decal && decal - 1 < static_cast<int>(lineCode.length()) && lineCode[decal - 1] == c1)
        {
            startLocation.column--;
            range++;
        }
        else if (cpt < 0 && decal + range - 1 < static_cast<int>(lineCode.length()) && lineCode[decal + range - 1] == c2)
        {
            range--;
        }
    }
}

void Diagnostic::sortRanges()
{
    std::ranges::sort(ranges, [](auto& r1, auto& r2) { return r1.startLocation.column < r2.startLocation.column; });
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
            std::swap(r.startLocation.column, r.endLocation.column);

        r.width = 1;
        if (r.endLocation.line == r.startLocation.line)
            r.width = r.endLocation.column - r.startLocation.column;
        else
            r.width = static_cast<int>(lineCode.length()) - r.startLocation.column;
        r.width = max(1, r.width);

        // Special case for a range == 1.
        if (r.width == 1 && r.startLocation.column < lineCode.count)
        {
            int decal = r.startLocation.column;

            // If this is a word, than take the whole word
            if ((lineCode[decal] & 0x80) == 0)
            {
                const bool isCWord = isalpha(lineCode[decal]) || lineCode[decal] == '_' || lineCode[decal] == '#' || lineCode[decal] == '@';
                if (isCWord)
                {
                    while (SWAG_IS_AL_NUM(lineCode[decal + 1]) || lineCode[decal + 1] == '_')
                    {
                        decal += 1;
                        r.width += 1;
                    }
                }

                // If this is an operator
#define IS_OP(__c) ((__c) == '>' || (__c) == '<' || (__c) == '=' || (__c) == '-' || (__c) == '+' || (__c) == '*' || (__c) == '/' || (__c) == '%')
                while (IS_OP(lineCode[decal]) && IS_OP(lineCode[decal + 1]))
                {
                    decal += 1;
                    r.width += 1;
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
    g_Log.writeEol();
}

void Diagnostic::collectSourceCode()
{
    auto location0 = startLocation;
    location0.line -= sourceFile->offsetGetLine;

    lineCode    = sourceFile->getLine(location0.line);
    lineCodeNum = location0.line + 1;
    minBlanks   = 0;

    // Remove blanks on the left, but keep indentation
    auto pz = lineCode.c_str();
    if (*pz && SWAG_IS_NOT_WIN_EOL(*pz) && SWAG_IS_NOT_EOL(*pz))
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

void Diagnostic::printSourceCode() const
{
    if (lineCode.empty())
        return;

    if (showErrorLevel || showFileName)
        printMargin(true);

    // Print all lines except the one that really contains the relevant stuff (and ranges)
    SyntaxColorContext cxt;
    cxt.mode = SyntaxColorMode::ForLog;

    const auto pz = lineCode.c_str();
    if (*pz == 0 || SWAG_IS_WIN_EOL(*pz) || SWAG_IS_EOL(*pz))
        return;
    printMargin(false, true, lineCodeNum);

    g_Log.setColor(LogColor::White);

    const auto colored = syntaxColor(lineCode.c_str() + minBlanks, cxt);
    g_Log.print(colored, true);
    g_Log.writeEol();
}

void Diagnostic::setColorRanges(DiagnosticLevel level) const
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
    }
}

void Diagnostic::alignRangeColumn(int& curColumn, int where, bool withCode) const
{
    while (curColumn < where)
    {
        if (withCode && curColumn < static_cast<int>(lineCode.count) && lineCode[curColumn] == '\t')
            g_Log.write("\t");
        else
            g_Log.write(" ");
        curColumn++;
    }
}

int Diagnostic::printRangesVerticalBars(size_t maxMarks)
{
    g_Log.writeEol();
    printMargin(false, true);
    int curColumn = minBlanks;
    for (uint32_t ii = 0; ii < maxMarks; ii++)
    {
        auto& rr  = ranges[ii];
        rr.hasBar = true;
        g_Log.setColor(rangeNoteColor);
        alignRangeColumn(curColumn, rr.mid);
        g_Log.print(LogSymbol::VerticalLine);
        curColumn++;
    }

    return curColumn;
}

void Diagnostic::printLastRangeHint(int curColumn)
{
    const auto& r = ranges.back();

    const auto leftColumn = curColumn;

    Vector<Utf8> tokens;
    const int    maxLength = g_CommandLine.errorRightColumn - leftColumn + minBlanks;
    Utf8::wordWrap(r.hint, tokens, max(maxLength, (int) g_CommandLine.errorRightColumn / 2));

    for (uint32_t i = 0; i < tokens.size(); i++)
    {
        g_Log.setColor(rangeNoteColor);
        g_Log.print(tokens[i]);
        if (i != tokens.size() - 1)
        {
            curColumn = printRangesVerticalBars(ranges.size() - 1);
            alignRangeColumn(curColumn, leftColumn, false);
        }
    }
}

void Diagnostic::printRanges()
{
    if (ranges.empty())
        return;

    printMargin(false, true);

    // Print all underlines
    int curColumn = minBlanks;
    for (uint32_t i = 0; i < ranges.size(); i++)
    {
        const auto& r = ranges[i];
        setColorRanges(r.errorLevel);
        alignRangeColumn(curColumn, r.startLocation.column);

        if (i != ranges.size() - 1 && r.mergeNext)
            setColorRanges(ranges[i + 1].errorLevel);

        while (curColumn < static_cast<int>(r.startLocation.column) + r.width && curColumn <= static_cast<int>(lineCode.length()) + 1)
        {
            curColumn++;
            g_Log.print(LogSymbol::HorizontalLine);
        }
    }

    // Remove all ranges with an empty message
    for (uint32_t i = 0; i < ranges.size(); i++)
    {
        if (ranges[i].hint.empty())
        {
            ranges.erase(ranges.begin() + i);
            i--;
        }
    }

    const auto orgNumRanges = ranges.size();

    // The last one in on the same line as the underline if there is enough room
    if (!ranges.empty())
    {
        const auto& r        = ranges.back();
        const auto  unformat = Log::removeFormat(r.hint.c_str());
        if (curColumn + 1 + unformat.length() < g_CommandLine.errorRightColumn)
        {
            g_Log.write(" ");
            printLastRangeHint(curColumn + 1);
            ranges.pop_back();
        }
    }

    while (!ranges.empty())
    {
        auto&      r        = ranges.back();
        auto       unformat = Log::removeFormat(r.hint.c_str());
        const auto mid      = r.mid - minBlanks;

        curColumn = printRangesVerticalBars(ranges.size() - 1);
        g_Log.setColor(rangeNoteColor);

        const bool notEnoughRoomRight = mid + 3 + static_cast<int>(unformat.length()) > static_cast<int>(g_CommandLine.errorRightColumn) || orgNumRanges >= 2;
        const bool enoughRoomLeft     = mid - 2 - static_cast<int>(unformat.length()) >= 0;

        // Can we stick the hint before the line reference ? (must be the last one)
        if (ranges.size() == 1 && notEnoughRoomRight && enoughRoomLeft)
        {
            alignRangeColumn(curColumn, r.mid - 2 - static_cast<int>(unformat.length()));
            g_Log.print(r.hint);
            g_Log.write(" ");
            g_Log.print(LogSymbol::HorizontalLine);
            g_Log.print(LogSymbol::DownLeft);
        }

        // The hint is the last one, and is too big to be on the right
        // So make it on its own line
        else if (ranges.size() == 1 && notEnoughRoomRight)
        {
            if (!ranges.back().hasBar)
            {
                alignRangeColumn(curColumn, r.mid);
                g_Log.print(LogSymbol::VerticalLineUp);
                g_Log.writeEol();
                printMargin(false, true);
                curColumn = minBlanks;
            }

            if (mid - 2 - static_cast<int>(unformat.length()) > -4)
                alignRangeColumn(curColumn, curColumn + 4);

            printLastRangeHint(curColumn);
        }
        else
        {
            alignRangeColumn(curColumn, r.mid);
            g_Log.print(LogSymbol::DownRight);
            g_Log.print(LogSymbol::HorizontalLine);
            g_Log.write(" ");
            curColumn += 3;
            printLastRangeHint(curColumn);
        }

        ranges.pop_back();
    }

    g_Log.writeEol();
}

void Diagnostic::report()
{
    // Message level
    if (showErrorLevel)
    {
        if (errorLevel == DiagnosticLevel::Note)
        {
            printMargin(false, true);
            printErrorLevel();

            Vector<Utf8> tokens;
            Utf8::wordWrap(textMsg, tokens, g_CommandLine.errorRightColumn);
            for (uint32_t i = 0; i < tokens.size(); i++)
            {
                g_Log.setColor(rangeNoteColor);
                g_Log.print(tokens[i]);
                if (i != tokens.size() - 1)
                {
                    g_Log.writeEol();
                    printMargin(false, true);
                }
            }

            showErrorLevel = false;
            g_Log.writeEol();
            if (hasContent)
            {
                printMargin(false, true);
                g_Log.writeEol();
            }
        }
        else
        {
            printErrorLevel();
            g_Log.print(textMsg);
            g_Log.writeEol();
        }
    }

    // Source file and location on their own line
    if (showFileName)
    {
        if (showErrorLevel)
            g_Log.writeEol();
        printMarginLineNo(0);
        g_Log.setColor(marginBorderColor);
        g_Log.print(LogSymbol::VerticalLine);
        g_Log.write(" ");
        g_Log.setColor(sourceFileColor);
        printSourceLine();
        g_Log.writeEol();
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

    auto str = formNte(Nte0177, typeInfo->getDisplayNameC());

    if (typeInfo->isAlias())
    {
        const auto typeAlias = castTypeInfo<TypeInfoAlias>(typeInfo, TypeInfoKind::Alias);
        if (typeAlias->rawType)
            str += form(" (aka [[%s]])", typeAlias->rawType->getConcreteAlias()->getDisplayNameC());
    }

    return str;
}

Utf8 Diagnostic::isType(const SymbolOverload* overload)
{
    if (!overload || !overload->typeInfo)
        return "";
    return formNte(Nte0148, Naming::kindName(overload).c_str(), overload->typeInfo->getDisplayNameC());
}

Utf8 Diagnostic::isType(const AstNode* node)
{
    if (node->resolvedSymbolOverload())
        return isType(node->resolvedSymbolOverload());
    return isType(node->typeInfo);
}

Diagnostic* Diagnostic::hereIs(const SymbolOverload* overload)
{
    if (!overload)
        return nullptr;
    return hereIs(overload->node);
}

Diagnostic* Diagnostic::hereIs(AstNode* node, const char* msg)
{
    if (!node)
        return nullptr;

    if (node->hasAstFlag(AST_GENERATED) &&
        node->token.isNot(TokenId::KwdPrivate) &&
        node->token.isNot(TokenId::KwdInternal) &&
        node->token.isNot(TokenId::KwdPublic))
        return nullptr;

    const Utf8 txt  = msg ? Utf8{msg} : toNte(Nte0062);
    const Utf8 txt1 = form(txt, Naming::kindName(node).c_str(), node->token.c_str());
    return note(node, node->getTokenName(), txt1);
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

void Diagnostic::removeErrorId(Utf8& err)
{
    if (hastErrorId(err))
    {
        err.remove(0, 10);
        err.trim();
    }
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
