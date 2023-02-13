#include "pch.h"
#include "Tokenizer.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Timer.h"
#include "LanguageSpec.h"

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
    return Report::report(diag);
}

void Tokenizer::appendTokenName(TokenParse& token)
{
    if (realAppendName)
        token.text.append(startTokenName, (int) (sourceFile->curBuffer - startTokenName));
    else
        token.text.setView(startTokenName, (int) (sourceFile->curBuffer - startTokenName));
}

void Tokenizer::setFile(SourceFile* file)
{
    location.column = 0;
    location.line   = 0;
    sourceFile      = file;
}

void Tokenizer::saveState(const TokenParse& token)
{
    st_token               = token;
    st_curBuffer           = sourceFile->curBuffer;
    st_location            = location;
    st_forceLastTokenIsEOL = forceLastTokenIsEOL;
}

void Tokenizer::restoreState(TokenParse& token)
{
    token                 = st_token;
    sourceFile->curBuffer = st_curBuffer;
    location              = st_location;
    forceLastTokenIsEOL   = st_forceLastTokenIsEOL;
}

void Tokenizer::processChar(uint32_t c)
{
    location.column++;

    if (c == '\n')
    {
        if (g_CommandLine.stats)
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

bool Tokenizer::doMultiLineComment(TokenParse& token)
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
            return error(token, Err(Tkn0025), Hnt(Hnt0041));
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

void Tokenizer::doIdentifier(TokenParse& token, uint32_t c, unsigned offset)
{
    while (SWAG_IS_ALPHA(c) || SWAG_IS_DIGIT(c) || c == '_')
    {
        treatChar(c, offset);
        c = getCharNoSeek(offset);
    }

    appendTokenName(token);

    auto it = g_LangSpec->keywords.find(token.text);
    if (it)
    {
        token.id = *it;
        if (token.id == TokenId::NativeType)
        {
            auto it1 = g_LangSpec->nativeTypes.find(token.text);
            SWAG_ASSERT(it1);
            token.literalType = *it1;
        }
    }
    else
    {
        token.id = TokenId::Identifier;
    }
}

bool Tokenizer::getToken(TokenParse& token)
{
    Timer timer(&g_Stats.tokenizerTime);
    if (g_CommandLine.stats)
        g_Stats.numTokens++;

    token.literalType      = LiteralType::TT_MAX;
    token.lastTokenIsEOL   = forceLastTokenIsEOL;
    token.lastTokenIsBlank = false;
    forceLastTokenIsEOL    = false;

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
            token.id          = TokenId::EndOfFile;
            token.endLocation = token.startLocation;
            return true;
        }

        // Blank
        ///////////////////////////////////////////
        if (SWAG_IS_EOL(c))
        {
            token.lastTokenIsEOL = true;
            continue;
        }

        // End of line
        ///////////////////////////////////////////
        if (SWAG_IS_BLANK(c))
        {
            token.lastTokenIsBlank = true;
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
                token.lastTokenIsEOL = true;
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

        // Comp�ler
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
                SWAG_CHECK(doStringLiteral(token, true, true));
                return true;
            }

            doIdentifier(token, nc, offset);
            token.endLocation = location;

            if (!token.text.count)
            {
                token.startLocation = location;
                return error(token, Fmt(Err(Err0141), nc));
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
            if (sourceFile->curBuffer[0] == '"' && sourceFile->curBuffer[1] == '"')
            {
                sourceFile->curBuffer += 2;
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
        return error(token, Fmt(Err(Err0081), token.ctext()));
    }

    return true;
}