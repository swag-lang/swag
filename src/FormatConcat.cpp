#include "pch.h"
#include "FormatConcat.h"

void FormatConcat::addChar(char c)
{
    ensureSpace(1);
    *currentSP++ = c;

    if (SWAG_IS_EOL(c))
    {
        eol++;
        blank = 0;
    }
    else if (!SWAG_IS_BLANK(c))
    {
        eol   = 0;
        blank = 0;
    }
    else
        blank++;
}

void FormatConcat::addString(const char* v, uint32_t len)
{
    ensureSpace(len);
    std::copy_n(v, len, currentSP);
    currentSP += len;
    eol   = 0;
    blank = 0;
}

void FormatConcat::addBlank(bool test)
{
    if (!test)
        return;
    addChar(' ');
}

void FormatConcat::addEol()
{
    if (eol)
        return;
    addChar('\n');
}

void FormatConcat::addIndent(uint32_t num)
{
    if (blank)
        return;
    while (num--)
    {
        addBlank();
        addBlank();
        addBlank();
        addBlank();
    }
}

void FormatConcat::addString(const Utf8& v)
{
    addString(v.c_str(), v.length());
}

void FormatConcat::addStringFormat(const char* format, ...)
{
    char    buf[4096];
    va_list args;
    va_start(args, format);
    const auto len = vsnprintf(buf, 4096, format, args);
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

Utf8 FormatConcat::getUtf8() const
{
    Utf8 result;
    for (auto b = firstBucket; b; b = b->nextBucket)
        result.append(reinterpret_cast<const char*>(b->data), bucketCount(b));
    return result;
}
