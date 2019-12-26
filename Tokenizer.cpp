#include "pch.h"
#include "Tokenizer.h"
#include "Stats.h"
#include "LanguageSpec.h"
#include "Diagnostic.h"
#include "SourceFile.h"

void Tokenizer::setFile(SourceFile* file)
{
    seek            = 0;
    location.column = 0;
    location.line   = 0;
    endReached      = false;
    cacheChar       = 0;
    sourceFile      = file;
    for (int i = 0; i < REPORT_NUM_CODE_LINES - 1; i++)
        location.seekStartLine[i] = -1;
    location.seekStartLine[REPORT_NUM_CODE_LINES - 1] = 0;
}

inline void Tokenizer::treatChar(char32_t c, unsigned offset)
{
    if (!c)
        return;

    cacheChar = 0;
    seek += offset;
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
        for (int i = 0; i < REPORT_NUM_CODE_LINES - 1; i++)
            location.seekStartLine[i] = location.seekStartLine[i + 1];
        location.seekStartLine[REPORT_NUM_CODE_LINES - 1] = seek;
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

inline char32_t Tokenizer::getChar(unsigned& offset, bool seekMove)
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

    if (seekMove)
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

bool Tokenizer::error(Token& token, const Utf8& msg)
{
    token.endLocation = location;
    sourceFile->report({sourceFile, token, msg});
    return false;
}

void Tokenizer::doDocComment(Token& token)
{
    token.id = TokenId::DocComment;

    while (true)
    {
        auto nc = getChar();
        if (!nc || nc == '\n')
            break;
        token.text += nc;
    }

    lastTokenIsEOL = true;
}

bool Tokenizer::getToken(Token& token, bool skipEOL)
{
    unsigned offset;
    lastTokenIsEOL      = forceLastTokenIsEOL;
    forceLastTokenIsEOL = false;
    while (true)
    {
        token.startLocation = location;
        token.literalType   = nullptr;
        token.text.clear();

        auto c = getChar();
        if (c == 0)
        {
            token.id = TokenId::EndOfFile;
            return true;
        }

        // Blank
        if (SWAG_IS_EOL(c))
        {
            if (!skipEOL)
            {
                token.id = TokenId::EndOfLine;
                return true;
            }

            lastTokenIsEOL = true;
            continue;
        }

        if (SWAG_IS_BLANK(c))
            continue;

        // Comments
        if (c == '/')
        {
            auto nc = getCharNoSeek(offset);

            // C++ comment //
            if (nc == '/')
            {
                treatChar(c, offset);
                nc = getChar();

                // This is a '///' doc comment
                if (nc == '/' && g_CommandLine.generateDoc)
                {
                    getChar();
                    doDocComment(token);
                    return true;
                }

                while (nc && nc != '\n')
                    nc = getChar();
                lastTokenIsEOL = true;
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
        if (SWAG_IS_ALPHA(c) || c == '_' || c == '#' || c == '@')
        {
            token.text = c;
            auto nc    = getCharNoSeek(offset);
            if (c == '#' && nc == '[')
            {
                treatChar(nc, offset);
                token.text += nc;
                token.endLocation = location;
                token.id          = TokenId::SymAttrStart;
                return true;
            }

            // Raw string literal
            if (c == '#' && nc == '"')
            {
                treatChar(nc, offset);
                token.text = nc;
                SWAG_CHECK(doStringLiteral(token, true));
                return true;
            }

            getIdentifier(token, nc, offset);
            token.endLocation = location;

            if (token.id == TokenId::Identifier && token.text[0] == '#')
            {
                sourceFile->report({sourceFile, token, format("invalid compiler command '%s'", token.text.c_str())});
                return false;
            }

            return true;
        }

        // Number literal
        if (SWAG_IS_DIGIT(c))
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

        if (c == '\'')
        {
            SWAG_CHECK(doCharLiteral(token));
            return true;
        }

        // Symbols
        if (doSymbol(c, token))
        {
            token.endLocation = location;
            return true;
        }

        // Unknown character
        token.endLocation = location;
        token.text        = c;
        token.id          = TokenId::Invalid;
        token.endLocation = location;
        sourceFile->report({sourceFile, token, format("invalid character '%s'", token.text.c_str())});
        return false;
    }

    return true;
}