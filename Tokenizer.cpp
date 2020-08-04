#include "pch.h"
#include "Tokenizer.h"
#include "Stats.h"
#include "LanguageSpec.h"
#include "Diagnostic.h"
#include "SourceFile.h"

const char* g_TokenNames[] =
{
#undef DEFINE_TOKEN_ID
#define DEFINE_TOKEN_ID(__id) #__id
#include "TokenizerIds.h"
};

void Tokenizer::setFile(SourceFile* file)
{
    seek            = 0;
    location.column = 0;
    location.line   = 0;
    endReached      = false;
    cacheChar[0]    = 0;
    cacheChar[1]    = 0;
    sourceFile      = file;
    for (int i = 0; i < REPORT_NUM_CODE_LINES - 1; i++)
        location.seekStartLine[i] = -1;
    location.seekStartLine[REPORT_NUM_CODE_LINES - 1] = 0;
}

inline void Tokenizer::treatChar(char32_t c, unsigned offset)
{
    if (!c)
        return;

    if (cacheChar[0])
    {
        cacheChar[0]       = cacheChar[1];
        cacheCharOffset[0] = cacheCharOffset[1];
        cacheChar[1]       = 0;
        cacheCharOffset[1] = 0;
    }

    seek += offset;
    location.column++;

    // End of line
    if (c == '\n')
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

inline char32_t Tokenizer::getChar(unsigned& offset, bool seekMove, bool useCache)
{
    if (endReached)
        return 0;

    // One character is already there, no need to read
    char32_t c = 0;
    offset     = 1;
    if (useCache && cacheChar[0])
    {
        c      = cacheChar[0];
        offset = cacheCharOffset[0];
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
    {
        treatChar(c, offset);
    }
    else if (useCache || !cacheChar[0])
    {
        cacheChar[0]       = c;
        cacheCharOffset[0] = offset;
    }
    else if (!cacheChar[1])
    {
        cacheChar[1]       = c;
        cacheCharOffset[1] = offset;
    }
    else
    {
        SWAG_ASSERT(false);
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

bool Tokenizer::getToken(Token& token)
{
    unsigned offset;

    if ((lastTokenIsEOL || forceLastTokenIsEOL) && (parseFlags & TOKENIZER_KEEP_EOL))
    {
        lastTokenIsEOL      = false;
        forceLastTokenIsEOL = false;
        token.id            = TokenId::EndOfLine;
        token.text          = "\n";
        return true;
    }

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
            if (parseFlags & TOKENIZER_KEEP_EOL)
            {
                token.text += c;
                token.id = TokenId::EndOfLine;
                return true;
            }

            lastTokenIsEOL = true;
            continue;
        }

        if (SWAG_IS_BLANK(c))
        {
            if (parseFlags & TOKENIZER_KEEP_BLANKS)
            {
                token.text += c;
                token.id = TokenId::Blank;
                return true;
            }

            continue;
        }

        // Comments
        if (c == '/')
        {
            auto nc = getCharNoSeek(offset);

            // C++ comment //
            if (nc == '/')
            {
                treatChar(c, offset);
                nc = getChar();

                // Keep comments alive
                if (parseFlags & TOKENIZER_KEEP_CPP_COMMENTS)
                {
                    token.id = TokenId::DocComment;
                    token.text += "//";
                    while (nc && nc != '\n')
                    {
                        if (nc != '\r')
                            token.text += nc;
                        nc = getChar();
                    }

                    lastTokenIsEOL = true;
                    return true;
                }

                // This is a '///' doc comment
                if (nc == '/' && (g_CommandLine.generateDoc || g_CommandLine.test))
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
                token.text.clear();
                SWAG_CHECK(doStringLiteral(token, true));
                return true;
            }

            if (c == '@' && nc == '[')
            {
                token.id = TokenId::SymLiteralBracket;
                return true;
            }

            if (c == '@' && nc == '{')
            {
                token.id = TokenId::SymLiteralCurly;
                return true;
            }

            if (c == '@' && nc == '(')
            {
                token.id = TokenId::SymLiteralParen;
                return true;
            }

            getIdentifier(token, nc, offset);
            token.endLocation = location;

            if (token.id == TokenId::Identifier && token.text[0] == '#')
            {
                sourceFile->report({sourceFile, token, format("unknown compiler instruction '%s'", token.text.c_str())});
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