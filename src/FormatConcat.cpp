#include "pch.h"
#include "FormatConcat.h"

void FormatConcat::addBlank(bool test)
{
    if(!test)
        return;
    ensureSpace(1);
    *currentSP++ = ' ';
}

void FormatConcat::addEol()
{
    ensureSpace(1);
    *currentSP++ = '\n';
}

void FormatConcat::addIndent(uint32_t num)
{
    while (num--)
    {
        ensureSpace(1);
        *currentSP++ = '\t';
    }
}

void FormatConcat::addChar(char c)
{
    SWAG_ASSERT(!SWAG_IS_BLANK(c));
    ensureSpace(1);
    *currentSP++ = c;
}

void FormatConcat::addString(const char* v, uint32_t len)
{
    ensureSpace(len);
    std::copy_n(v, len, currentSP);
    currentSP += len;
}

void FormatConcat::addString(const Utf8& v)
{
    const auto len = v.length();
    ensureSpace(len);
    std::copy_n(v.buffer, len, currentSP);
    currentSP += len;
}

void FormatConcat::addU32Str(uint32_t value)
{
    if (value < 10)
        addChar(static_cast<char>(value + '0'));
    else
        addString(std::to_string(value));
}

void FormatConcat::addStringFormat(const char* format, ...)
{
    char    buf[4096];
    va_list args;
    va_start(args, format);
    const auto len = vsnprintf(buf, 4096, format, args);
    SWAG_ASSERT(len < 4095);
    va_end(args);
    addStringN(buf, len);
}
