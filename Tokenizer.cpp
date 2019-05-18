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

inline unsigned Tokenizer::getChar()
{
    // One character is already there, no need to read
    unsigned c = 0;
    if (m_cacheChar)
    {
        c           = m_cacheChar;
        m_cacheChar = 0;
    }
    else
        c = m_sourceFile->getChar();

    if (!c)
    {
        if (!m_endReached)
        {
            m_endReached = true;
            g_Stats.numLines++;
        }

        return 0;
    }

    m_location.column++;
    m_location.seek++;

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
            Diagnostic diag(m_sourceFile, token.startLocation, token.endLocation, L"unexpected end of file found in comment");
            m_sourceFile->report(diag);
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
    auto c = getChar();
    while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')
    {
        token.text += c;
        c = getChar();
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
                if (!ZapCComment(token))
                    return false;
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
            return true;
        }

        token.text        = c;
        token.id          = TokenId::Invalid;
        token.endLocation = m_location;
        Diagnostic diag(m_sourceFile, token.startLocation, token.endLocation, format(L"invalid character '%s'", token.text.c_str()), DiagnosticLevel::Error);
        m_sourceFile->report(diag);
        return false;
    }

    return true;
}