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
    m_location.seek          = 0;
    m_location.column        = 0;
    m_location.line          = 0;
    m_location.seekStartLine = 0;
    m_endReached             = false;
    m_cacheChar              = 0;
    m_sourceFile             = file;
}

inline void Tokenizer::treatChar(char32_t c, unsigned offset)
{
    if (!c)
        return;

    m_cacheChar = 0;
    m_location.seek += offset;
    m_location.column++;

    // Align tabulations
    if (c == '\t')
    {
        while (m_location.column % g_CommandLine.tabSize)
            m_location.column++;
    }

    // End of line
    else if (c == '\n')
    {
        g_Stats.numLines++;
        m_location.column = 0;
        m_location.line++;
        m_location.seekStartLine = m_location.seek;
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
    if (m_endReached)
        return 0;

    // One character is already there, no need to read
    char32_t c = 0;
    offset     = 1;
    if (m_cacheChar)
    {
        c           = m_cacheChar;
        offset      = m_cacheOffset;
        m_cacheChar = 0;
    }
    else
    {
        c = m_sourceFile->getChar(offset);
    }

    if (!c)
    {
        if (!m_endReached)
        {
            m_endReached = true;
            g_Stats.numLines++;
        }

        return 0;
    }

    if (seek)
        treatChar(c, offset);
    else
    {
        m_cacheChar   = c;
        m_cacheOffset = offset;
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
            m_sourceFile->report({m_sourceFile, token, "unexpected end of file found in comment"});
            return false;
        }

        if (nc == '*')
        {
            nc = getChar();
            if (nc == '/')
            {
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

    // Keyword
    auto it = g_LangSpec.m_keywords.find(token.text);
    if (it != g_LangSpec.m_keywords.end())
        token.id = it->second;
    else
        token.id = TokenId::Identifier;

    // Type
    if (token.id == TokenId::NativeType)
        token.numType = g_LangSpec.m_nativeTypes[token.text];
}

bool Tokenizer::error(Token& token, const utf8& msg)
{
    token.endLocation = m_location;
    m_sourceFile->report({m_sourceFile, token, msg});
    return false;
}

bool Tokenizer::getToken(Token& token)
{
    unsigned offset;
    while (true)
    {
        token.startLocation = m_location;
        auto c              = getChar();
        if (c == 0)
        {
            token.id = TokenId::EndOfFile;
            return true;
        }

        // Blank
        if (c == '\n' || c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r')
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
            token.endLocation = m_location;
            if (token.id == TokenId::Identifier && token.text[0] == '#')
            {
                m_sourceFile->report({m_sourceFile, token, format("invalid compiler command '%s'", token.text.c_str())});
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

        // Symbols
        token.endLocation = m_location;
        if (doSymbol(c, token))
            return true;

        // Unknown character
        token.text        = c;
        token.id          = TokenId::Invalid;
        token.endLocation = m_location;
        m_sourceFile->report({m_sourceFile, token, format("invalid character '%s'", token.text.c_str())});
        return false;
    }

    return true;
}