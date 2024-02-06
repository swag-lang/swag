#include "pch.h"
#include "Tokenizer.h"
#include "Diagnostic.h"
#include "ErrorContext.h"
#include "ErrorIds.h"
#include "SourceFile.h"
#include "Statistics.h"
#ifdef SWAG_STATS
#include "Timer.h"
#endif

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

bool Tokenizer::error(TokenParse& token, const Utf8& msg, const Utf8& hint) const
{
    token.endLocation = location;

    Diagnostic diag{sourceFile, token, msg};
    diag.hint = hint;
    return errorContext->report(diag);
}

void Tokenizer::appendTokenName(TokenParse& token) const
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

    for (int i       = 'a'; i <= (int) 'z'; i++)
        idLetters[i] = true;
    for (int i       = 'A'; i <= (int) 'Z'; i++)
        idLetters[i] = true;
    for (int i       = '0'; i <= (int) '9'; i++)
        idLetters[i] = true;
    idLetters[(int) '_'] = true;
}

void Tokenizer::saveState(const TokenParse& token)
{
    st_token               = token;
    st_curBuffer           = curBuffer;
    st_location            = location;
    st_forceLastTokenIsEOL = forceLastTokenIsEOL;
    st_comment             = comment;
}

void Tokenizer::restoreState(TokenParse& token)
{
    token               = st_token;
    curBuffer           = st_curBuffer;
    location            = st_location;
    forceLastTokenIsEOL = st_forceLastTokenIsEOL;
    comment             = st_comment;
}

uint32_t Tokenizer::peekChar(unsigned& offset) const
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
    unsigned   offset;
    const auto c = peekChar(offset);
    eatChar(c, offset);
    return c;
}

void Tokenizer::processChar(uint32_t c, unsigned offset)
{
    if (c == '\n')
    {
#ifdef SWAG_STATS
        ++g_Stats.numLines;
#endif
        location.column = 0;
        location.line++;
    }
    else
    {
        // Don't really know why if this is utf8 of 3 bytes, we should just increase by 2
        // for consoles to have a correct column indentation. Is this because of windows UTF16 ?
        location.column += min(offset, 2);
    }
}

void Tokenizer::eatChar(uint32_t c, unsigned offset)
{
    processChar(c, offset);
    curBuffer += offset;
}

TokenId Tokenizer::tokenRelated(TokenId id)
{
    switch (id)
    {
    case TokenId::SymRightParen:
        return TokenId::SymLeftParen;
    case TokenId::SymRightSquare:
        return TokenId::SymLeftSquare;
    case TokenId::SymRightCurly:
        return TokenId::SymLeftCurly;
    case TokenId::SymLeftParen:
        return TokenId::SymRightParen;
    case TokenId::SymLeftSquare:
        return TokenId::SymRightSquare;
    case TokenId::SymLeftCurly:
        return TokenId::SymRightCurly;
    default:
        break;
    }

    SWAG_ASSERT(false);
    return TokenId::SymQuestion;
}

bool Tokenizer::nextToken(TokenParse& token)
{
#ifdef SWAG_STATS
    Timer timer(&g_Stats.tokenizerTime);
    ++g_Stats.numTokens;
#endif

    token.literalType   = LiteralType::TT_MAX;
    bool hasEol         = forceLastTokenIsEOL;
    token.flags         = forceLastTokenIsEOL ? TOKENPARSE_LAST_EOL : 0;
    forceLastTokenIsEOL = false;

    if (!propagateComment)
        comment.clear();
    propagateComment = true;

    while (true)
    {
        token.text.clear();
        startTokenName      = curBuffer;
        token.startLocation = location;

        const auto c = readChar();

        // End of file
        ///////////////////////////////////////////
        if (c == 0)
        {
            token.id          = TokenId::EndOfFile;
            token.text        = "<end of file>";
            token.endLocation = token.startLocation;
            return true;
        }

        // End of line
        ///////////////////////////////////////////
        if (SWAG_IS_EOL(c))
        {
            while (SWAG_IS_EOL(curBuffer[0]))
                readChar();
            token.flags |= TOKENPARSE_LAST_EOL;
            hasEol = true;
            comment.clear();
            continue;
        }

        // Blank
        ///////////////////////////////////////////
        if (SWAG_IS_BLANK(c))
        {
            while (SWAG_IS_BLANK(curBuffer[0]))
                readChar();
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

                if (hasEol)
                {
                    token.flags |= TOKENPARSE_EOL_BEFORE_COMMENT;
                    hasEol = false;
                }

                startTokenName = curBuffer;
                while (curBuffer[0] && !SWAG_IS_EOL(curBuffer[0]))
                    readChar();

                if (curBuffer[0])
                {
                    token.flags |= TOKENPARSE_LAST_EOL;
                    readChar();
                }

                if (trackComments)
                {
                    appendTokenName(token);
                    comment += token.text;
                    if (comment.back() != '\n')
                        comment += "\n";

                    // In case of end of line comments, skip all blanks after
                    if (!(token.flags & TOKENPARSE_EOL_BEFORE_COMMENT))
                    {
                        while (curBuffer[0] && (SWAG_IS_EOL(curBuffer[0]) || SWAG_IS_BLANK(curBuffer[0])))
                            readChar();
                    }
                }

                continue;
            }

            // Multiline comment
            if (curBuffer[0] == '*')
            {
                readChar();

                startTokenName = curBuffer;
                SWAG_CHECK(doMultiLineComment(token));

                if (trackComments)
                {
                    appendTokenName(token);
                    comment += token.text;
                    comment.removeBack();
                    comment.removeBack();

                    // In case of end of line comments, skip all blanks after
                    if (!(token.flags & TOKENPARSE_EOL_BEFORE_COMMENT))
                    {
                        while (curBuffer[0] && (SWAG_IS_EOL(curBuffer[0]) || SWAG_IS_BLANK(curBuffer[0])))
                        {
                            if (SWAG_IS_EOL(curBuffer[0]))
                            {
                                token.flags |= TOKENPARSE_LAST_EOL;
                                hasEol = true;
                            }

                            readChar();
                        }
                    }
                }

                continue;
            }
        }

        // Compîler
        ///////////////////////////////////////////
        if (c == '#')
        {
            if (curBuffer[0] == '"')
            {
                readChar();
                token.literalType = LiteralType::TT_STRING_RAW;
                SWAG_CHECK(doStringLiteral(token));
                return true;
            }

            if (curBuffer[0] == '[')
            {
                readChar();
                token.text = "#[";
                token.id   = TokenId::SymAttrStart;
            }
            else
            {
                SWAG_CHECK(doIdentifier(token));
            }

            token.endLocation = location;
            return true;
        }

        // Intrinsic
        ///////////////////////////////////////////
        if (c == '@')
        {
            SWAG_CHECK(doIdentifier(token));
            token.endLocation = location;
            return true;
        }

        // Identifier
        ///////////////////////////////////////////
        if (SWAG_IS_ALPHA(c) || c == '_')
        {
            SWAG_CHECK(doIdentifier(token));
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

        // Character literal
        ///////////////////////////////////////////
        if (c == '`')
        {
            SWAG_CHECK(doCharacterLiteral(token));
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
                token.literalType = LiteralType::TT_STRING_MULTILINE;
                SWAG_CHECK(doStringLiteral(token));
            }
            else
            {
                token.literalType = LiteralType::TT_STRING;
                SWAG_CHECK(doStringLiteral(token));
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
        return error(token, FMT(Err(Err0234), token.ctext()));
    }

    return true;
}
