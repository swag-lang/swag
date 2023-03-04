#include "pch.h"
#include "Tokenizer.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Timer.h"
#include "LanguageSpec.h"
#include "ErrorContext.h"
#include "Statistics.h"

const char* g_TokenNames[] =
    {
#undef DEFINE_TOKEN_ID
#define DEFINE_TOKEN_ID(__id, __flags) #__id,
#include "TokenIds.h"
};

const uint32_t g_TokenFlags[] =
    {
#undef DEFINE_TOKEN_ID
#define DEFINE_TOKEN_ID(__id, __flags) __flags,
#include "TokenIds.h"
};

bool Tokenizer::error(TokenParse& token, const Utf8& msg, const Utf8& hint)
{
    token.endLocation = location;

    Diagnostic diag{sourceFile, token, msg};
    diag.hint = hint;
    return errorContext->report(diag);
}

void Tokenizer::appendTokenName(TokenParse& token)
{
    if (realAppendName)
        token.text.append(startTokenName, (int) (curBuffer - startTokenName));
    else
        token.text.setView(startTokenName, (int) (curBuffer - startTokenName));
}

void Tokenizer::setup(ErrorContext* errorCxt, SourceFile* file)
{
    SWAG_ASSERT(!curBuffer);

    errorContext    = errorCxt;
    location.column = 0;
    location.line   = 0;
    sourceFile      = file;
    curBuffer       = sourceFile->buffer + sourceFile->offsetStartBuffer;
    endBuffer       = sourceFile->buffer + sourceFile->bufferSize;

    for (int i = (int) 'a'; i <= (int) 'z'; i++)
        idLetters[i] = true;
    for (int i = (int) 'A'; i <= (int) 'Z'; i++)
        idLetters[i] = true;
    for (int i = (int) '0'; i <= (int) '9'; i++)
        idLetters[i] = true;
    idLetters[(int) '_'] = true;
}

void Tokenizer::saveState(const TokenParse& token)
{
    st_token               = token;
    st_curBuffer           = curBuffer;
    st_location            = location;
    st_forceLastTokenIsEOL = forceLastTokenIsEOL;
}

void Tokenizer::restoreState(TokenParse& token)
{
    token               = st_token;
    curBuffer           = st_curBuffer;
    location            = st_location;
    forceLastTokenIsEOL = st_forceLastTokenIsEOL;
}

uint32_t Tokenizer::peekChar(unsigned& offset)
{
    if (curBuffer >= endBuffer)
    {
        offset = 0;
        return 0;
    }

    if ((*curBuffer & 0x80) == 0)
    {
        offset = 1;
        return *curBuffer;
    }

    uint32_t wc;
    Utf8::decodeUtf8(curBuffer, wc, offset);
    return wc;
}

uint32_t Tokenizer::readChar()
{
    unsigned offset;
    auto     c = peekChar(offset);
    eatChar(c, offset);
    return c;
}

void Tokenizer::processChar(uint32_t c)
{
    if (c == '\n')
    {
#ifdef SWAG_STATS
        g_Stats.numLines++;
#endif
        location.column = 0;
        location.line++;
    }
    else
    {
        location.column++;
    }
}

void Tokenizer::eatChar(uint32_t c, unsigned offset)
{
    processChar(c);
    curBuffer += offset;
}

Utf8 Tokenizer::tokenToName(TokenId id)
{
    switch (id)
    {
    case TokenId::SymComma:
        return ",";
    case TokenId::SymColon:
        return ":";
    case TokenId::SymSemiColon:
        return ";";
    case TokenId::SymLeftCurly:
        return "{";
    case TokenId::SymRightCurly:
        return "}";
    case TokenId::SymLeftParen:
        return "(";
    case TokenId::SymRightParen:
        return ")";
    case TokenId::SymLeftSquare:
        return "[";
    case TokenId::SymRightSquare:
        return "]";
    case TokenId::SymDot:
        return ".";
    case TokenId::SymExclam:
        return "!";
    case TokenId::SymMinusGreat:
        return "->";
    case TokenId::SymEqual:
        return "=";
    case TokenId::SymVertical:
        return "|";
    }

    return "???";
}

bool Tokenizer::nextToken(TokenParse& token)
{
#ifdef SWAG_STATS
    Timer timer(&g_Stats.tokenizerTime);
    g_Stats.numTokens++;
#endif

    token.literalType   = LiteralType::TT_MAX;
    token.flags         = forceLastTokenIsEOL ? TOKENPARSE_LAST_EOL : 0;
    forceLastTokenIsEOL = false;

    while (true)
    {
        token.text.clear();
        startTokenName      = curBuffer;
        token.startLocation = location;

        auto c = readChar();

        // End of file
        ///////////////////////////////////////////
        if (c == 0)
        {
            token.id          = TokenId::EndOfFile;
            token.endLocation = token.startLocation;
            return true;
        }

        // Blank
        ///////////////////////////////////////////
        if (SWAG_IS_EOL(c))
        {
            while (SWAG_IS_EOL(curBuffer[0]))
                c = readChar();
            token.flags |= TOKENPARSE_LAST_EOL;
            continue;
        }

        // End of line
        ///////////////////////////////////////////
        if (SWAG_IS_BLANK(c))
        {
            while (SWAG_IS_BLANK(curBuffer[0]))
                c = readChar();
            token.flags |= TOKENPARSE_LAST_BLANK;
            continue;
        }

        // Comments
        ///////////////////////////////////////////
        if (c == '/')
        {
            // Line comment
            if (curBuffer[0] == '/')
            {
                readChar();
                while (curBuffer[0] && !SWAG_IS_EOL(curBuffer[0]))
                    readChar();
                continue;
            }

            // Multiline comment
            if (curBuffer[0] == '*')
            {
                readChar();
                SWAG_CHECK(doMultiLineComment(token));
                continue;
            }
        }

        // Comp�ler
        ///////////////////////////////////////////
        if (c == '#')
        {
            if (curBuffer[0] == '[')
            {
                readChar();
                token.text = "#[";
                token.id   = TokenId::SymAttrStart;
            }
            else
            {
                doIdentifier(token);
            }

            token.endLocation = location;
            return true;
        }

        // Intrinsic
        ///////////////////////////////////////////
        if (c == '@')
        {
            if (curBuffer[0] == '"')
            {
                readChar();
                SWAG_CHECK(doStringLiteral(token, true, true));
                return true;
            }

            doIdentifier(token);
            token.endLocation = location;

            if (!token.text.count)
            {
                token.startLocation = location;
                return error(token, Fmt(Err(Err0141), curBuffer[0]));
            }

            return true;
        }

        // Identifier
        ///////////////////////////////////////////
        if (SWAG_IS_ALPHA(c) || c == '_')
        {
            doIdentifier(token);
            token.endLocation = location;
            return true;
        }

        // Number literal
        ///////////////////////////////////////////
        if (SWAG_IS_DIGIT(c))
        {
            SWAG_CHECK(doNumberLiteral(token, c));
            return true;
        }

        // String literal
        ///////////////////////////////////////////
        if (c == '"')
        {
            if (curBuffer[0] == '"' && curBuffer[1] == '"')
            {
                readChar();
                readChar();
                SWAG_CHECK(doStringLiteral(token, false, true));
            }
            else
            {
                SWAG_CHECK(doStringLiteral(token, false, false));
            }

            return true;
        }

        // Symbols
        ///////////////////////////////////////////
        if (doSymbol(token, c))
        {
            token.endLocation = location;
            appendTokenName(token);
            return true;
        }

        // Unknown character
        ///////////////////////////////////////////
        token.text = c;
        token.id   = TokenId::Invalid;
        return error(token, Fmt(Err(Err0081), token.ctext()));
    }

    return true;
}