#include "pch.h"
#include "Tokenizer.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "ErrorIds.h"
#include "Timer.h"

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

bool Tokenizer::isSymbol(TokenId id)
{
    return g_TokenFlags[(int) id] & TOKEN_SYM;
}

bool Tokenizer::isLiteral(TokenId id)
{
    return g_TokenFlags[(int) id] & TOKEN_LITERAL;
}

void Tokenizer::appendTokenName(Token& token)
{
    token.text.append(startTokenName, (int)(sourceFile->curBuffer - startTokenName));
    //token.text.setView(startTokenName, (int) (sourceFile->curBuffer - startTokenName));
}

void Tokenizer::setFile(SourceFile* file)
{
    location.column = 0;
    location.line   = 0;
    sourceFile      = file;
}

void Tokenizer::processChar(uint32_t c)
{
    location.column++;

    if (c == '\n')
    {
        if (g_CommandLine->stats)
            g_Stats.numLines++;
        location.column = 0;
        location.line++;
    }
}

void Tokenizer::treatChar(uint32_t c, unsigned offset)
{
    processChar(c);
    sourceFile->curBuffer += offset;
}

uint32_t Tokenizer::getChar()
{
    unsigned offset;
    auto     c = sourceFile->getChar(offset);
    treatChar(c, offset);
    return c;
}

uint32_t Tokenizer::getCharNoSeek(unsigned& offset)
{
    return sourceFile->getChar(offset);
}

bool Tokenizer::error(Token& token, const Utf8& msg)
{
    token.endLocation = location;
    sourceFile->report({sourceFile, token, msg});
    return false;
}

bool Tokenizer::doMultiLineComment(Token& token)
{
    int countEmb = 1;
    while (true)
    {
        auto nc = getChar();
        while (nc && nc != '*' && nc != '/')
            nc = getChar();

        if (!nc)
        {
            location = token.startLocation;
            location.column += 2;
            error(token, Msg0080);
            return false;
        }

        if (nc == '*')
        {
            unsigned offset;
            nc = getCharNoSeek(offset);
            if (nc == '/')
            {
                treatChar(nc, offset);
                countEmb--;
                if (countEmb == 0)
                    return true;
            }

            continue;
        }

        if (nc == '/')
        {
            nc = getChar();
            if (nc == '*')
            {
                countEmb++;
                continue;
            }
        }
    }
}

bool Tokenizer::getToken(Token& token)
{
    Timer timer(&g_Stats.tokenizerTime);

    lastTokenIsEOL      = forceLastTokenIsEOL;
    forceLastTokenIsEOL = false;
    lastTokenIsBlank    = false;

    token.literalType = LiteralType::TT_MAX;

    unsigned offset;
    while (true)
    {
        token.text.clear();
        startTokenName      = sourceFile->curBuffer;
        token.startLocation = location;

        auto c = getChar();

        // End of file
        ///////////////////////////////////////////
        if (c == 0)
        {
            token.id = TokenId::EndOfFile;
            return true;
        }

        // Blank
        ///////////////////////////////////////////
        if (SWAG_IS_EOL(c))
        {
            lastTokenIsEOL = true;
            continue;
        }

        // End of line
        ///////////////////////////////////////////
        if (SWAG_IS_BLANK(c))
        {
            lastTokenIsBlank = true;
            continue;
        }

        // Comments
        ///////////////////////////////////////////
        if (c == '/')
        {
            auto nc = getCharNoSeek(offset);

            // Line comment
            if (nc == '/')
            {
                treatChar(c, offset);
                nc = getChar();
                while (nc && nc != '\n')
                    nc = getChar();
                lastTokenIsEOL = true;
                continue;
            }

            // Multiline comment
            if (nc == '*')
            {
                treatChar(c, offset);
                SWAG_CHECK(doMultiLineComment(token));
                continue;
            }
        }

        // Compîler
        ///////////////////////////////////////////
        if (c == '#')
        {
            auto nc = getCharNoSeek(offset);

            if (nc == '[')
            {
                token.text = "#[";
                treatChar(nc, offset);
                token.endLocation = location;
                token.id          = TokenId::SymAttrStart;
                return true;
            }

            doIdentifier(token, nc, offset);
            token.endLocation = location;

            if (token.id == TokenId::Identifier)
            {
                error(token, Utf8::format(Msg0140, token.text.c_str()));
                return false;
            }

            return true;
        }

        // Intrinsic
        ///////////////////////////////////////////
        if (c == '@')
        {
            auto nc = getCharNoSeek(offset);

            if (nc == '"')
            {
                treatChar(nc, offset);
                SWAG_CHECK(doStringLiteral(token, true));
                return true;
            }

            if (nc == '[')
            {
                token.text = "@[";
                token.id   = TokenId::SymLiteralBracket;
                return true;
            }

            if (nc == '{')
            {
                token.text = "@{";
                token.id   = TokenId::SymLiteralCurly;
                return true;
            }

            if (nc == '(')
            {
                token.text = "@(";
                token.id   = TokenId::SymLiteralParen;
                return true;
            }

            doIdentifier(token, nc, offset);
            token.endLocation = location;

            if (!token.text.count)
            {
                token.startLocation = location;
                error(token, Utf8::format(Msg0141, nc));
                return false;
            }

            return true;
        }

        // Identifier
        ///////////////////////////////////////////
        if (SWAG_IS_ALPHA(c) || c == '_')
        {
            auto nc = getCharNoSeek(offset);
            doIdentifier(token, nc, offset);
            token.endLocation = location;
            return true;
        }

        // Number literal
        ///////////////////////////////////////////
        if (SWAG_IS_DIGIT(c))
        {
            SWAG_CHECK(doNumberLiteral(c, token));
            return true;
        }

        // String literal
        ///////////////////////////////////////////
        if (c == '"')
        {
            SWAG_CHECK(doStringLiteral(token, false));
            return true;
        }

        // Symbols
        ///////////////////////////////////////////
        if (doSymbol(c, token))
        {
            token.endLocation = location;
            appendTokenName(token);
            return true;
        }

        // Unknown character
        ///////////////////////////////////////////
        token.text = c;
        token.id   = TokenId::Invalid;
        error(token, Utf8::format(Msg0081, token.text.c_str()));
        return false;
    }

    return true;
}