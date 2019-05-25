#include "pch.h"
#include "Tokenizer.h"
#include "SourceFile.h"
#include "Global.h"
#include "Stats.h"
#include "LanguageSpec.h"
#include "Diagnostic.h"
#include "CommandLine.h"

void Tokenizer::setFile(SourceFile* file)
{
    location.seek          = 0;
    location.column        = 0;
    location.line          = 0;
    location.seekStartLine = 0;
    endReached             = false;
    cacheChar              = 0;
    sourceFile             = file;
}

inline void Tokenizer::treatChar(char32_t c, unsigned offset)
{
    if (!c)
        return;

    cacheChar = 0;
    location.seek += offset;
    location.column++;

    // Align tabulations
    if (c == '\t')
    {
        while (location.column % g_CommandLine.tabSize)
            location.column++;
    }

    // End of line
    else if (c == '\n')
    {
        if (g_CommandLine.stats)
            g_Stats.numLines++;
        location.column = 0;
        location.line++;
        location.seekStartLine = location.seek;
    }
}

inline char32_t Tokenizer::getChar()
{
    unsigned offset;
    return getChar(offset, true);
}

inline char32_t Tokenizer::getCharNoSeek(unsigned& offset)
{
    return getChar(offset, false);
}

inline char32_t Tokenizer::getChar(unsigned& offset, bool seek)
{
    if (endReached)
        return 0;

    // One character is already there, no need to read
    char32_t c = 0;
    offset     = 1;
    if (cacheChar)
    {
        c         = cacheChar;
        offset    = cacheCharOffset;
        cacheChar = 0;
    }
    else
    {
        c = sourceFile->getChar(offset);
    }

    if (!c)
    {
        if (!endReached)
        {
            endReached = true;
            if (g_CommandLine.stats)
                g_Stats.numLines++;
        }

        return 0;
    }

    if (seek)
        treatChar(c, offset);
    else
    {
        cacheChar       = c;
        cacheCharOffset = offset;
    }

    return c;
}

bool Tokenizer::eatCComment(Token& token)
{
    int countEmb = 1;
    while (true)
    {
        auto nc = getChar();
        while (nc && nc != '*' && nc != '/')
            nc = getChar();

        if (!nc)
        {
            token.endLocation = token.startLocation;
            token.endLocation.column += 2;
            sourceFile->report({sourceFile, token, "unexpected end of file found in comment"});
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

void Tokenizer::getIdentifier(Token& token)
{
    unsigned offset;
    auto     c = getCharNoSeek(offset);
    while (SWAG_IS_ALPHA(c) || SWAG_IS_DIGIT(c) || c == '_')
    {
        token.text += c;
        treatChar(c, offset);
        c = getCharNoSeek(offset);
    }

    // Special keywords
    if (token.text == "true")
    {
        token.id             = TokenId::LiteralNumber;
        token.literalType    = &g_TypeInfoBool;
        token.literalValue.b = true;
        return;
    }

    if (token.text == "false")
    {
        token.id             = TokenId::LiteralNumber;
        token.literalType    = &g_TypeInfoBool;
        token.literalValue.b = false;
        return;
    }

    // Keyword
    auto it = g_LangSpec.keywords.find(token.text);
    if (it != g_LangSpec.keywords.end())
        token.id = it->second;
    else
        token.id = TokenId::Identifier;

    // Type
    if (token.id == TokenId::NativeType)
        token.literalType = g_LangSpec.nativeTypes[token.text];
}

bool Tokenizer::error(Token& token, const Utf8& msg)
{
    token.endLocation = location;
    sourceFile->report({sourceFile, token, msg});
    return false;
}

bool Tokenizer::getToken(Token& token, bool skipEOL)
{
    unsigned offset;
    while (true)
    {
        token.startLocation = location;
        token.literalType   = nullptr;

        auto c = getChar();
        if (c == 0)
        {
            token.id = TokenId::EndOfFile;
            return true;
        }

        // Blank
        if (c == '\n')
        {
            if (!skipEOL)
            {
                token.id = TokenId::EndOfLine;
                return true;
            }

            continue;
        }

        if (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r')
            continue;

        if (c == '/')
        {
            auto nc = getCharNoSeek(offset);

            // C++ comment //
            if (nc == '/')
            {
                treatChar(c, offset);
                nc = getChar();
                while (nc && nc != '\n')
                    nc = getChar();
                continue;
            }

            // C comment /*
            if (nc == '*')
            {
                treatChar(c, offset);
                SWAG_CHECK(eatCComment(token));
                continue;
            }
        }

        // Identifier
        if (SWAG_IS_ALPHA(c) || c == '_' || c == '#')
        {
            token.text = c;
            getIdentifier(token);
            token.endLocation = location;
            if (token.id == TokenId::Identifier && token.text[0] == '#')
            {
                sourceFile->report({sourceFile, token, format("invalid compiler command '%s'", token.text.c_str())});
                return false;
            }

            return true;
        }

        // Number literal
        if (SWAG_IS_DIGIT(c) || c == '.')
        {
            SWAG_CHECK(doNumberLiteral(c, token));
            return true;
        }

        // String literal
        if (c == '"')
        {
            SWAG_CHECK(doStringLiteral(token, false));
            return true;
        }

        if (c == '`')
        {
            SWAG_CHECK(doStringLiteral(token, true));
            return true;
        }

        if (c == '\'')
        {
            SWAG_CHECK(doCharLiteral(token));
            return true;
        }

        // Symbols
        token.endLocation = location;
        if (doSymbol(c, token))
            return true;

        // Unknown character
        token.text        = c;
        token.id          = TokenId::Invalid;
        token.endLocation = location;
        sourceFile->report({sourceFile, token, format("invalid character '%s'", token.text.c_str())});
        return false;
    }

    return true;
}