#include "pch.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Log.h"
#include "Semantic/Symbol/Symbol.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Syntax/SyntaxColor.h"
#include "Syntax/Tokenizer/Tokenizer.h"
#include "Wmf/SourceFile.h"

constexpr int MAX_INDENT_BLANKS = 10;

void Diagnostic::setupColors()
{
    errorColor                = LogColor::Red;
    errorColorHint            = LogColor::Red;
    errorColorHintHighLight   = LogColor::DarkRed;
    warningColor              = LogColor::Magenta;
    warningColorHint          = LogColor::Magenta;
    warningColorHintHighLight = LogColor::DarkMagenta;
    noteColorHint             = LogColor::White;
    noteColorHintHighLight    = LogColor::Gray;
    noteTitleColor            = LogColor::DarkYellow;
    noteColor                 = LogColor::White;
    marginBorderColor         = LogColor::Cyan;
    codeLineNoColor           = LogColor::Cyan;
    stackColor                = LogColor::DarkYellow;
    preRemarkColor            = LogColor::White;
    remarkColor               = LogColor::Gray;
    autoRemarkColor           = LogColor::Gray;
    sourceFileColor           = LogColor::Cyan;
}

void Diagnostic::setup()
{
    if (!sourceFile || sourceFile->path.empty())
        showFileName = false;
    if (!sourceFile || sourceFile->path.empty() || !hasLocation)
        showSourceCode = false;
    hasContent = showSourceCode || !remarks.empty() || !autoRemarks.empty() || !preRemarks.empty();
    textMsg    = preprocess(textMsg);
}

namespace
{
    void addThe(Utf8& replace, const Utf8& verb)
    {
        replace.replace(form(" %s identifier ", verb.cstr()), form(" %s the identifier ", verb.cstr()));
        replace.replace(form(" %s type ", verb.cstr()), form(" %s the type ", verb.cstr()));
        replace.replace(form(" %s literal ", verb.cstr()), form(" %s the literal ", verb.cstr()));
        replace.replace(form(" %s intrinsic ", verb.cstr()), form(" %s the intrinsic ", verb.cstr()));
        replace.replace(form(" %s keyword ", verb.cstr()), form(" %s the keyword ", verb.cstr()));
        replace.replace(form(" %s compiler instruction ", verb.cstr()), form(" %s the compiler instruction ", verb.cstr()));
        replace.replace(form(" %s symbol ", verb.cstr()), form(" %s the symbol ", verb.cstr()));
    }

    void addArticle(Utf8& replace, const Utf8& verb, const Utf8& a, const Utf8& an)
    {
        replace.replace(form(" %s $$A$$ ", verb.cstr()), form(" %s %s ", verb.cstr(), a.cstr()));
        replace.replace(form(" %s $$AN$$ ", verb.cstr()), form(" %s %s ", verb.cstr(), an.cstr()));
    }
}

Utf8 Diagnostic::preprocess(const Utf8& textMsg)
{
    Utf8 replace = textMsg;

    replace = replaceHighLight(replace);

    addArticle(replace, "expected", "a", "an");
    addArticle(replace, "got", "a", "an");
    addArticle(replace, "found", "a", "an");
    addArticle(replace, "with", "a", "an");
    addArticle(replace, "or", "a", "an");
    addArticle(replace, "and", "a", "an");

    addArticle(replace, "after", "the", "the");
    addArticle(replace, "before", "the", "the");

    replace.replace("$$A$$ ", "");
    replace.replace("$$AN$$ ", "");

    addThe(replace, "got");
    addThe(replace, "found");
    addThe(replace, "with");
    addThe(replace, "or");
    addThe(replace, "and");
    addThe(replace, "after");
    addThe(replace, "before");
    addThe(replace, "expected");

    replace.replace("[[1]] arguments", "[[1]] argument");
    replace.replace("[[1]] generic arguments", "[[1]] generic argument");
    replace.replace("[[1]] parameters", "[[1]] parameter");
    replace.replace("[[1]] values", "[[1]] value");
    replace.replace("[[1]] fields", "[[1]] field");
    replace.replace("[[1]] variables", "[[1]] variable");

    return replace;
}

Utf8 Diagnostic::replaceHighLight(const Utf8& textMsg)
{
    int idx = textMsg.find(" [[");
    if (idx == -1)
        return textMsg;

    Utf8 replace{textMsg.buffer, static_cast<uint32_t>(idx)};
    while (idx != -1)
    {
        bool isSymbol = true;

        Utf8 inside;
        idx += 3;

        int nextIdx = textMsg.find("]]", idx);
        if (nextIdx != -1)
        {
            while (nextIdx + 2 < static_cast<int>(textMsg.length()) && textMsg[nextIdx + 2] == ']')
                nextIdx++;

            while (idx != nextIdx)
            {
                const auto c = textMsg[idx++];
                inside += c;
                if (SWAG_IS_AL_NUM(c))
                    isSymbol = false;
            }

            if (isSymbol && inside.length() <= 1)
            {
                if (inside == ',')
                    replace += " $$A$$ [[comma]] ','";
                else if (inside == ';')
                    replace += " $$A$$ [[semicolon]] ';'";
                else if (inside == '(')
                    replace += " $$A$$ [[left parenthesis]] '('";
                else if (inside == ')')
                    replace += " $$A$$ [[right parenthesis]] ')'";
                else if (inside == ':')
                    replace += " $$A$$ [[colon]] ':'";
                else if (inside == '{')
                    replace += " $$A$$ [[left curly bracket]] '{'";
                else if (inside == '}')
                    replace += " $$A$$ [[right curly bracket]] '}'";
                else if (inside == '[')
                    replace += " $$A$$ [[left square bracket]] '['";
                else if (inside == ']')
                    replace += " $$A$$ [[right square bracket]] ']'";
                else if (inside == '_')
                    replace += " $$AN$$ [[underscore]] '_'";
                else
                {
                    replace += " $$A$$ symbol [['";
                    replace += inside;
                    replace += "']]";
                }
            }
            else if (isSymbol && (inside[0] != '\'' || inside.back() != '\''))
            {
                replace += " [[\'";
                replace += inside;
                replace += "\']]";
            }
            else
            {
                replace += " [[";
                replace += inside;
                replace += "]]";
            }

            idx += 2;
        }
        else
        {
            replace += " [[";
        }

        nextIdx = textMsg.find(" [[", idx);
        if (nextIdx == -1)
            replace += Utf8{textMsg.buffer + idx, textMsg.length() - idx};
        else
            replace += Utf8{textMsg.buffer + idx, static_cast<uint32_t>(nextIdx - idx)};
        idx = nextIdx;
    }

    return replace;
}

void Diagnostic::replaceTokenName(const Token& token)
{
    if (Tokenizer::isKeyword(token.id))
        textMsg.replaceAll("$$TKN$$", form("keyword [[%s]]", token.cstr()));
    else if (Tokenizer::isCompiler(token.id) || token.text.startsWith("#mix") || token.text.startsWith("#alias"))
        textMsg.replaceAll("$$TKN$$", form("compiler instruction [[%s]]", token.cstr()));
    else if (Tokenizer::isIntrinsicReturn(token.id))
        textMsg.replaceAll("$$TKN$$", form("intrinsic [[%s]]", token.cstr()));
    else if (Tokenizer::isSymbol(token.id))
        textMsg.replaceAll("$$TKN$$", form("[[%s]]", token.cstr()));
    else if (Tokenizer::isLiteral(token.id))
        textMsg.replaceAll("$$TKN$$", form("literal [[%s]]", token.cstr()));
    else if (token.is(TokenId::NativeType))
        textMsg.replaceAll("$$TKN$$", form("type [[%s]]", token.cstr()));
    else if (token.is(TokenId::Identifier))
        textMsg.replaceAll("$$TKN$$", form("identifier [[%s]]", token.cstr()));
    else if (Tokenizer::isIntrinsicNoReturn(token.id))
        textMsg.replaceAll("$$TKN$$", form("intrinsic [[%s]]", token.cstr()));
    else
        textMsg.replaceAll("$$TKN$$", form("[[%s]]", token.cstr()));
}

void Diagnostic::replaceTokenName(const TokenParse& tokenParse)
{
    if (Tokenizer::isLiteral(tokenParse.token.id) && tokenParse.literalType == LiteralType::TypeString)
        textMsg.replace("$$TKN$$", form("literal [[\"%s\"]]", tokenParse.cstr()));
    else if (Tokenizer::isLiteral(tokenParse.token.id) && tokenParse.literalType == LiteralType::TypeCharacter)
        textMsg.replace("$$TKN$$", form("literal [[`%s`]]", tokenParse.cstr()));
    else
        replaceTokenName(tokenParse.token);
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
    if (msg.empty())
        return;
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

void Diagnostic::addNote(const AstNode* node, const Utf8& h)
{
    if (!node)
        return;

    SourceLocation start, end;
    node->computeLocation(start, end);
    notes.push_back(note(node->token.sourceFile, start, end, h));
}

void Diagnostic::printSourceLine(Log* log) const
{
    SWAG_ASSERT(sourceFile);
    auto checkFile = sourceFile;
    if (checkFile->fileForSourceLocation)
        checkFile = checkFile->fileForSourceLocation;

    Path path = checkFile->path;
    if (!g_CommandLine.errorAbsolute)
        path = path.filename();

    log->print(path);
    if (hasLocation)
        log->print(form(":%d:%d:%d:%d: ", startLocation.line + 1, startLocation.column + 1, endLocation.line + 1, endLocation.column + 1));
    else
        log->write(": ");
}

void Diagnostic::printMarginLineNo(Log* log, uint32_t lineNo) const
{
    log->setColor(codeLineNoColor);

    auto numDigits = 0;
    auto l         = lineNo;
    while (l)
    {
        l /= 10;
        numDigits++;
    }

    uint32_t m = lineNo ? numDigits : 0;
    while (m++ < lineCodeMaxDigits + 1)
        log->write(" ");
    if (lineNo)
        log->print(form("%d", lineNo));
    log->write(" ");
}

void Diagnostic::printMargin(Log* log, bool eol, bool printLineNo, uint32_t lineNo) const
{
    if (!printLineNo)
    {
        if (eol)
            log->writeEol();
        return;
    }

    printMarginLineNo(log, lineNo);

    log->setColor(marginBorderColor);
    if (fromContext)
        log->print(LogSymbol::VerticalLineDot);
    else
        log->print(LogSymbol::VerticalLine);
    log->write(" ");

    if (eol)
        log->writeEol();
}

void Diagnostic::printErrorLevel(Log* log)
{
    // Put the error ID right after the error level, instead at the beginning of the message
    Utf8 id;
    if (hasErrorId(textMsg))
    {
        id = Utf8(textMsg.buffer, 9);
        textMsg.remove(0, 10);
    }

    switch (errorLevel)
    {
        case DiagnosticLevel::Error:
            log->setColor(errorColor);
            log->write("error: ");
            break;

        case DiagnosticLevel::Panic:
            log->setColor(errorColor);
            log->write("panic: ");
            break;

        case DiagnosticLevel::Warning:
            log->setColor(warningColor);
            log->write("warning: ");
            break;

        case DiagnosticLevel::Note:
            log->setColor(noteTitleColor);
            log->write("note: ");
            log->setColor(noteColor);
            break;
    }

    if (!id.empty())
    {
        log->print(id);
        log->write(": ");
    }
}

void Diagnostic::printPreRemarks(Log* log) const
{
    if (!preRemarks.empty())
    {
        for (const auto& r : preRemarks)
        {
            if (r.empty())
                continue;
            printMargin(log, false, true, 0);
            if (r.empty() || r[0] == ' ')
            {
                log->setColor(remarkColor);
                log->write(" ");
            }
            else
            {
                log->setColor(preRemarkColor);
                log->print(LogSymbol::DotList);
            }

            log->write(" ");
            log->print(r);
            log->writeEol();
        }
    }
}

void Diagnostic::printRemarks(Log* log) const
{
    if (!autoRemarks.empty())
    {
        for (const auto& r : autoRemarks)
        {
            if (r.empty())
                continue;
            printMargin(log, false, true, 0);
            log->setColor(autoRemarkColor);
            log->print(LogSymbol::DotList);
            log->write(" ");
            log->print(r);
            log->writeEol();
        }
    }

    if (!remarks.empty())
    {
        for (const auto& r : remarks)
        {
            if (r.empty())
                continue;
            printMargin(log, false, true, 0);
            log->setColor(remarkColor);
            log->print(LogSymbol::DotList);
            log->write(" ");
            log->print(r);
            log->writeEol();
        }
    }
}

namespace
{
    void fixRange(const Utf8& lineCode, SourceLocation& startLocation, uint32_t& range, char c1, char c2)
    {
        if (range == 1)
            return;

        const auto decal = startLocation.column;
        int        cpt   = 0;

        for (uint32_t i = decal; i < lineCode.length() && i < decal + range; i++)
        {
            if (lineCode[i] == c1)
                cpt++;
            else if (lineCode[i] == c2)
                cpt--;
        }

        if (cpt > 0 && decal + range < lineCode.length() && lineCode[decal + range] == c2)
        {
            range++;
        }
        else if (cpt > 0 && decal < lineCode.length() && lineCode[decal] == c1)
        {
            startLocation.column++;
            SWAG_ASSERT(range);
            range--;
        }
        else if (cpt < 0 && decal && decal - 1 < lineCode.length() && lineCode[decal - 1] == c1)
        {
            startLocation.column--;
            range++;
        }
        else if (cpt < 0 && decal + range - 1 < lineCode.length() && lineCode[decal + range - 1] == c2)
        {
            SWAG_ASSERT(range);
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
        r.hint = preprocess(r.hint);

        // Be sure start column is before end column
        if (r.startLocation.line == r.endLocation.line && r.startLocation.column > r.endLocation.column)
            std::swap(r.startLocation.column, r.endLocation.column);

        r.width = 1;
        if (r.endLocation.line == r.startLocation.line)
        {
            SWAG_ASSERT(r.endLocation.column >= r.startLocation.column);
            r.width = r.endLocation.column - r.startLocation.column;
        }
        else
        {
            SWAG_ASSERT(lineCode.length() >= r.startLocation.column);
            r.width = lineCode.length() - r.startLocation.column;
        }

        r.width = max(1, r.width);

        // Special case for a range == 1.
        if (r.width == 1 && r.startLocation.column < lineCode.count)
        {
            uint32_t decal = r.startLocation.column;

            // If this is a word, then take the whole word
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
        if (r.width && ((r.width & 1) == 0))
            r.mid--;
    }
}

void Diagnostic::reportCompact(Log* log)
{
    setupColors();
    printErrorLevel(log);

    Vector<Utf8> tokens;
    tokenizeError(textMsg, tokens);

    log->print(tokens[0]);
    log->print(": ");

    log->setColor(sourceFileColor);
    printSourceLine(log);

    if (tokens.size() > 1)
    {
        log->setColor(LogColor::White);
        log->print(tokens[1]);
    }

    log->writeEol();
    log->setDefaultColor();
}

void Diagnostic::collectSourceCode()
{
    auto location0 = startLocation;
    location0.line -= sourceFile->offsetGetLine;

    lineCode    = sourceFile->getLine(location0.line);
    lineCodeNum = location0.line;
    minBlanks   = 0;

    // Remove blanks on the left, but keep indentation
    const auto pz = lineCode.cstr();
    if (*pz && SWAG_IS_NOT_WIN_EOL(*pz) && SWAG_IS_NOT_EOL(*pz))
    {
        const auto countBlanks = lineCode.countStartBlanks();
        if (countBlanks > MAX_INDENT_BLANKS)
            minBlanks = countBlanks - MAX_INDENT_BLANKS;

        // Is error at the start of the line ?
        if (location0.line && location0.column - countBlanks == 0)
        {
            lineCodeNumPrev = location0.line;
            while (lineCodeNumPrev)
            {
                lineCodePrev = sourceFile->getLine(lineCodeNumPrev - 1);
                lineCodeNumPrev--;
                auto prev = lineCodePrev;
                prev.trim();
                if (!prev.empty())
                {
                    const uint32_t countPrevBlanks = lineCodePrev.countStartBlanks();
                    if (countPrevBlanks > MAX_INDENT_BLANKS)
                        minBlanks = min(minBlanks, countPrevBlanks - MAX_INDENT_BLANKS);
                    break;
                }
            }
        }
    }
}

void Diagnostic::printSourceCode(Log* log, uint32_t num, const Utf8& line) const
{
    if (line.empty())
        return;

    // Print all lines except the one that really contains the relevant stuff (and ranges)
    SyntaxColorContext cxt;
    cxt.mode = SyntaxColorMode::ForLog;

    const auto pz = line.cstr();
    if (*pz == 0 || SWAG_IS_WIN_EOL(*pz) || SWAG_IS_EOL(*pz))
        return;
    printMargin(log, false, true, num);

    log->setColor(LogColor::White);

    const auto      colored = doSyntaxColor(line.cstr() + minBlanks, cxt);
    LogWriteContext logCxt;
    logCxt.raw = true;
    log->print(colored, &logCxt);
    log->writeEol();
}

void Diagnostic::printSourceCode(Log* log) const
{
    if (showErrorLevel || showFileName)
        printMargin(log, true);

    printSourceCode(log, lineCodeNumPrev + 1, lineCodePrev);
    printSourceCode(log, lineCodeNum + 1, lineCode);
}

void Diagnostic::setColorRanges(Log* log, DiagnosticLevel level, HintPart part, LogWriteContext* logCxt) const
{
    switch (level)
    {
        case DiagnosticLevel::Error:
        case DiagnosticLevel::Panic:
            if (part == HintPart::Underline)
                log->setColor(errorColor);
            else if (part == HintPart::Arrow)
                log->setColor(noteColor);
            else if (part == HintPart::ErrorLevel)
                log->setColor(errorColorHint);
            else
                log->setColor(errorColorHint);
            if (logCxt)
                logCxt->colorHighlight = Log::colorToVTS(errorColorHintHighLight) + Log::colorToVTS(LogColor::Underline);
            break;
        case DiagnosticLevel::Warning:
            if (part == HintPart::Underline)
                log->setColor(warningColor);
            else if (part == HintPart::Arrow)
                log->setColor(noteColor);
            else if (part == HintPart::ErrorLevel)
                log->setColor(warningColorHint);
            else
                log->setColor(warningColorHint);
            if (logCxt)
                logCxt->colorHighlight = Log::colorToVTS(warningColorHintHighLight) + Log::colorToVTS(LogColor::Underline);
            break;
        case DiagnosticLevel::Note:
            if (part == HintPart::Underline)
                log->setColor(noteColor);
            else if (part == HintPart::Arrow)
                log->setColor(noteColor);
            else if (part == HintPart::ErrorLevel)
                log->setColor(noteColor);
            else
                log->setColor(noteColorHint);
            if (logCxt)
                logCxt->colorHighlight = Log::colorToVTS(noteColorHintHighLight);
            break;
    }
}

void Diagnostic::alignRangeColumn(Log* log, uint32_t& curColumn, uint32_t where, bool withCode) const
{
    if (curColumn >= where)
        return;

    while (curColumn < where)
    {
        if (withCode && curColumn < lineCode.count && lineCode[curColumn] == '\t')
            log->write("\t");
        else
            log->write(" ");
        curColumn++;
    }
}

uint32_t Diagnostic::printRangesVerticalBars(Log* log, size_t maxMarks)
{
    log->writeEol();
    printMargin(log, false, true);
    uint32_t curColumn = minBlanks;
    for (uint32_t ii = 0; ii < maxMarks; ii++)
    {
        auto& rr  = ranges[ii];
        rr.hasBar = true;
        setColorRanges(log, rr.errorLevel, HintPart::Arrow);
        alignRangeColumn(log, curColumn, rr.mid);
        log->print(LogSymbol::VerticalLine);
        curColumn++;
    }

    return curColumn;
}

void Diagnostic::printLastRangeHint(Log* log, uint32_t curColumn)
{
    const auto& r          = ranges.back();
    const auto  leftColumn = curColumn;

    Vector<Utf8>   tokens;
    const uint32_t maxLength = g_CommandLine.errorRightColumn - leftColumn + minBlanks;
    Utf8::wordWrap(r.hint, tokens, max(maxLength, g_CommandLine.errorRightColumn / 2));

    for (uint32_t i = 0; i < tokens.size(); i++)
    {
        LogWriteContext logCxt;
        setColorRanges(log, r.errorLevel, HintPart::Text, &logCxt);
        log->print(tokens[i], &logCxt);

        if (i != tokens.size() - 1)
        {
            curColumn = printRangesVerticalBars(log, ranges.size() - 1);
            alignRangeColumn(log, curColumn, leftColumn, false);
        }
    }
}

void Diagnostic::printRanges(Log* log)
{
    if (ranges.empty())
        return;

    printMargin(log, false, true);

    // Print all underlines
    uint32_t curColumn = minBlanks;
    for (uint32_t i = 0; i < ranges.size(); i++)
    {
        const auto& r = ranges[i];
        setColorRanges(log, r.errorLevel, HintPart::Underline);
        alignRangeColumn(log, curColumn, r.startLocation.column);

        if (i != ranges.size() - 1 && r.mergeNext)
            setColorRanges(log, ranges[i + 1].errorLevel, HintPart::Underline);

        while (curColumn < r.startLocation.column + r.width && curColumn <= lineCode.length() + 1)
        {
            curColumn++;
            log->print(LogSymbol::HorizontalLine);
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

    while (!ranges.empty())
    {
        auto&      r        = ranges.back();
        auto       unFormat = Log::removeFormat(r.hint.cstr());
        const auto mid      = static_cast<int>(r.mid - minBlanks);

        curColumn = printRangesVerticalBars(log, ranges.size() - 1);
        setColorRanges(log, r.errorLevel, HintPart::Arrow);

        const bool notEnoughRoomRight = mid + 3 + static_cast<int>(unFormat.length()) > static_cast<int>(g_CommandLine.errorRightColumn) || orgNumRanges >= 2;
        const bool enoughRoomLeft     = mid - 2 - static_cast<int>(unFormat.length()) >= 0;

        // Can we stick the hint before the line reference ? (must be the last one)
        if (ranges.size() == 1 && notEnoughRoomRight && enoughRoomLeft)
        {
            alignRangeColumn(log, curColumn, r.mid - 2 - static_cast<int>(unFormat.length()));
            log->print(r.hint);
            log->write(" ");
            log->print(LogSymbol::HorizontalLine);
            log->print(LogSymbol::DownLeft);
        }

        // The hint is the last one, and is too big to be on the right
        // So make it on its own line
        else if (ranges.size() == 1 && notEnoughRoomRight)
        {
            if (!ranges.back().hasBar)
            {
                alignRangeColumn(log, curColumn, r.mid);
                log->print(LogSymbol::VerticalLineUp);
                log->writeEol();
                printMargin(log, false, true);
                curColumn = minBlanks;
            }

            if (mid - 2 - static_cast<int>(unFormat.length()) > -4)
                alignRangeColumn(log, curColumn, curColumn + 4);

            printLastRangeHint(log, curColumn);
        }
        else
        {
            alignRangeColumn(log, curColumn, r.mid);
            log->print(LogSymbol::DownRight);
            log->print(LogSymbol::HorizontalLine);
            log->write(" ");
            curColumn += 3;
            printLastRangeHint(log, curColumn);
        }

        ranges.pop_back();

        if (!ranges.empty())
            printRangesVerticalBars(log, ranges.size());
    }

    log->writeEol();
}

void Diagnostic::report(Log* log)
{
    // Message level
    if (showErrorLevel)
    {
        if (errorLevel == DiagnosticLevel::Note)
        {
            printMargin(log, false, true);
            printErrorLevel(log);

            Vector<Utf8> tokens;
            Utf8::wordWrap(textMsg, tokens, g_CommandLine.errorRightColumn);
            for (uint32_t i = 0; i < tokens.size(); i++)
            {
                log->setColor(noteColorHint);
                log->print(tokens[i]);
                if (i != tokens.size() - 1)
                {
                    log->writeEol();
                    printMargin(log, false, true);
                }
            }

            showErrorLevel = false;
            log->writeEol();
            if (hasContent)
            {
                printMargin(log, false, true);
                log->writeEol();
            }
        }
        else
        {
            printErrorLevel(log);

            LogWriteContext logCxt;
            setColorRanges(log, errorLevel, HintPart::ErrorLevel, &logCxt);
            log->print(textMsg, &logCxt);
            log->writeEol();
        }
    }

    // Source file and location on their own line
    if (showFileName)
    {
        if (showErrorLevel)
            log->writeEol();
        printMarginLineNo(log, 0);
        log->setColor(marginBorderColor);
        if (fromContext)
            log->print(LogSymbol::VerticalLineDot);
        else
            log->print(LogSymbol::VerticalLine);
        log->write(" ");
        log->setColor(sourceFileColor);
        printSourceLine(log);
        log->writeEol();
        printMargin(log, false, true);
    }

    // Code pre remarks
    if (!preRemarks.empty())
    {
        printPreRemarks(log);
        printMargin(log, true, true);
    }

    // Source code
    if (showSourceCode)
    {
        printSourceCode(log);
        printRanges(log);
    }

    // Code remarks
    if (!autoRemarks.empty() || !remarks.empty())
    {
        printMargin(log, true, true);
        printRemarks(log);
    }

    log->setDefaultColor();
}

Utf8 Diagnostic::isType(TypeInfo* typeInfo)
{
    if (!typeInfo)
        return "";

    auto str = formNte(Nte0216, typeInfo->getDisplayNameC());

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
    return formNte(Nte0160, Naming::kindName(overload).cstr(), overload->typeInfo->getDisplayNameC());
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

    if (msg)
    {
        const auto txt  = msg;
        const Utf8 txt1 = form(txt, Naming::kindName(node).cstr(), node->token.cstr());
        return note(node, node->getTokenName(), txt1);
    }

    const auto txt  = toNte(Nte0180);
    const Utf8 txt1 = form(txt, Naming::kindName(node).cstr(), node->token.cstr());
    return note(node, node->getTokenName(), txt1);
}

bool Diagnostic::hasErrorId(const Utf8& textMsg)
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

void Diagnostic::removeErrorId(Utf8& textMsg)
{
    if (hasErrorId(textMsg))
    {
        textMsg.remove(0, 10);
        textMsg.trimLeft();
    }
}

Utf8 Diagnostic::getErrorId(const Utf8& textMsg)
{
    if (!hasErrorId(textMsg))
        return "";
    return Utf8{textMsg.buffer + 1, 7};
}

void Diagnostic::tokenizeError(const Utf8& err, Vector<Utf8>& tokens)
{
    const char* pz = err.cstr();
    while (*pz)
    {
        Utf8 result;
        if (!pz[0] || !pz[1] || !pz[2])
        {
            while (pz[0])
                result += *pz++;
        }
        else
        {
            while (pz[0] && (pz[0] != ' ' || pz[1] != '$' || pz[2] != ' '))
                result += *pz++;
        }

        result.trim();
        if (!result.empty())
            tokens.push_back(result);
        if (*pz)
            pz += 3;
    }
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

bool Diagnostic::containsText(const Utf8& txt) const
{
    if (textMsg.containsNoCase(txt))
        return true;

    for (const auto& n : notes)
    {
        if (n->textMsg.containsNoCase(g_CommandLine.verboseErrorsFilter))
            return true;
    }

    for (const auto& n : remarks)
    {
        if (n.containsNoCase(g_CommandLine.verboseErrorsFilter))
            return true;
    }

    for (const auto& n : autoRemarks)
    {
        if (n.containsNoCase(g_CommandLine.verboseErrorsFilter))
            return true;
    }

    return false;
}
