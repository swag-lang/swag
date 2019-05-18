#include "pch.h"
#include "Tokenizer.h"
#include "SourceFile.h"
#include "Global.h"
#include "Stats.h"
#include "LanguageSpec.h"
#include "Diagnostic.h"
#include "CommandLine.h"

void Tokenizer::setFile(class SourceFile* file)
{
    m_location.seek   = 0;
    m_location.column = 0;
    m_location.line   = 0;
    m_endReached      = false;
    m_cacheChar       = 0;
    m_sourceFile      = file;
}

inline void Tokenizer::treatChar(unsigned c)
{
    if (!c)
        return;

    m_location.seek++;
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

inline unsigned Tokenizer::getChar(bool seek)
{
    // One character is already there, no need to read
    unsigned c = 0;
    if (m_cacheChar)
    {
        c           = m_cacheChar;
        m_cacheChar = 0;
    }
    else
    {
        c = m_sourceFile->getChar();
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
        treatChar(c);
    return c;
}

bool Tokenizer::ZapCComment(Token& token)
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
            m_sourceFile->report({m_sourceFile, token, L"unexpected end of file found in comment"});
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

void Tokenizer::GetIdentifier(Token& token)
{
    auto c = getChar(false);
    while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')
    {
        token.text += c;
        treatChar(c);
        c = getChar(false);
    }

    m_cacheChar = c;

    auto it = g_LangSpec.m_keywords.find(token.text);
    if (it != g_LangSpec.m_keywords.end())
        token.id = it->second;
    else
        token.id = TokenId::Identifier;
}

bool Tokenizer::getToken(Token& token)
{
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
            auto nc = getChar();

            // C++ comment //
            if (nc == '/')
            {
                nc = getChar();
                while (nc && nc != '\n')
                    nc = getChar();
                continue;
            }

            // C comment /*
            if (nc == '*')
            {
                SWAG_CHECK(ZapCComment(token));
                continue;
            }

            token.endLocation = m_location;
            token.id          = TokenId::SymSlash;
            return true;
        }

        // Identifier
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '#')
        {
            token.text = c;
            GetIdentifier(token);
            token.endLocation = m_location;
            if (token.id == TokenId::Identifier && token.text[0] == '#')
            {
                m_sourceFile->report({m_sourceFile, token, format(L"invalid compiler command '%s'", token.text.c_str())});
                return false;
            }

            return true;
        }

        token.text        = c;
        token.id          = TokenId::Invalid;
        token.endLocation = m_location;
        m_sourceFile->report({m_sourceFile, token, format(L"invalid character '%s'", token.text.c_str())});
        return false;
    }

    return true;
}