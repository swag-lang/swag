#include "pch.h"
#include "Format/FormatConcat.h"

void FormatConcat::addChar(uint8_t c)
{
    ensureSpace(1);
    *currentSP++ = c;
    column++;
    indent = 0;

    if (SWAG_IS_EOL(c))
    {
        eol++;
        totalEol++;
        blank  = 0;
        column = 0;
    }
    else if (!SWAG_IS_BLANK(c))
    {
        eol   = 0;
        blank = 0;
    }
    else
        blank++;
}

void FormatConcat::addBlank()
{
    addChar(' ');
}

void FormatConcat::alignToColumn(uint32_t destCol)
{
    while (column < destCol)
        addChar(' ');
}

void FormatConcat::addEol()
{
    if (eol || noEol)
        return;
    addChar('\n');
}

void FormatConcat::addBlankLine()
{
    if (eol == 1)
        addChar('\n');
}

bool FormatConcat::removeLastChar(uint8_t c)
{
    if (currentSP == lastBucket->data)
    {
        if (lastBucket == firstBucket)
            return false;

        auto b = firstBucket;
        while (b->nextBucket != lastBucket)
            b = b->nextBucket;
        if (!b->countBytes)
            return false;

        lastBucket = b;
        currentSP  = lastBucket->data + lastBucket->countBytes;
    }

    if (currentSP[-1] == c || c == 0)
    {
        column--; // this is false, depends on 'c', but is it enough for the current usage?
        currentSP--;
        return true;
    }

    return false;
}

void FormatConcat::addIndent(uint32_t num)
{
    if (!eol)
        return;
    if (indent == num)
        return;

    if (indent > num)
    {
        while (indent > num)
        {
            indent--;
            for (uint32_t i = 0; i < 4; i++)
                removeLastChar(' ');
        }
    }
    else
    {
        alignToColumn(num * 4);
        indent = num;
    }
}

void FormatConcat::addString(const std::string_view& view)
{
    addString(view.data(), static_cast<uint32_t>(view.length()));
}

void FormatConcat::addString(const char* v, uint32_t len)
{
    ensureSpace(len);
    std::copy_n(v, len, currentSP);
    currentSP += len;
    eol   = 0;
    blank = 0;
    column += len;
    indent = 0;
}

void FormatConcat::addStringFormat(const char* format, ...)
{
    char    buf[4096];
    va_list args;
    va_start(args, format);
    const auto len = vsnprintf(buf, 4096, format, args); // NOLINT(clang-diagnostic-format-nonliteral)
    SWAG_ASSERT(len < 4095);
    va_end(args);
    addString(buf, len);
}

void FormatConcat::addU32Str(uint32_t value)
{
    if (value < 10)
        addChar(static_cast<char>(value + '0'));
    else
        addString(std::to_string(value));
}

void FormatConcat::clear()
{
    Concat::clear();

    totalEol = 0;
    eol      = 0;
    blank    = 0;
    column   = 0;
    noEol    = 0;
}

Utf8 FormatConcat::getUtf8() const
{
    Utf8 result;
    for (auto b = firstBucket; b; b = b->nextBucket)
        result.append(reinterpret_cast<const char*>(b->data), bucketCount(b));
    return result;
}

uint32_t FormatConcat::length() const
{
    uint32_t result = 0;
    for (auto b = firstBucket; b != lastBucket->nextBucket; b = b->nextBucket)
        result += bucketCount(b);
    return result;
}
