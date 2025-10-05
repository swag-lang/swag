#include <utility>

#include "pch.h"
#include "Backend/Runtime.h"
#include "Core/Crc32.h"
#include "Main/Statistics.h"

Utf8::~Utf8()
{
    freeBuffer();
}

void Utf8::freeBuffer() const
{
    Allocator::free(buffer, allocated);
#ifdef SWAG_STATS
    g_Stats.memUtf8 -= allocated;
#endif
}

void Utf8::release()
{
    freeBuffer();
    buffer    = nullptr;
    count     = 0;
    allocated = 0;
}

Utf8::Utf8(const char* from)
{
    const uint32_t len = from ? static_cast<uint32_t>(strlen(from)) : 0;
    if (!len)
        return;

    reserve(len + 1);
    std::copy_n(from, len + 1, buffer);
    count = len;
}

Utf8::Utf8(const SwagSlice& slice)
{
    if (!slice.buffer || !slice.count)
        return;
    reserve(static_cast<uint32_t>(slice.count) + 1);
    std::copy_n(static_cast<uint8_t*>(slice.buffer), slice.count, buffer);
    buffer[slice.count] = 0;
    count               = static_cast<uint32_t>(slice.count);
}

Utf8::Utf8(const char* from, uint32_t len)
{
    if (!from || !len)
        return;
    reserve(len + 1);
    std::copy_n(from, len, buffer);
    buffer[len] = 0;
    count       = len;
}

Utf8::Utf8(const std::string& from)
{
    const uint32_t len = static_cast<uint32_t>(from.length());
    if (!len)
        return;

    reserve(len + 1);
    std::copy_n(from.c_str(), len + 1, buffer);
    count = len;
}

Utf8::Utf8(const Utf8& from)
{
    const uint32_t len = from.count;
    if (!len)
        return;

    // View
    if (!from.allocated && from.buffer)
    {
        count     = from.count;
        allocated = from.allocated;
        buffer    = from.buffer;
        return;
    }

    reserve(len + 1);
    std::copy_n(from.buffer, len + 1, buffer);
    count = len;
}

Utf8::Utf8(const Utf8& from, uint32_t capacity)
{
    const uint32_t len = from.count;
    if (!len)
        return;

    // View
    if (!from.allocated && from.buffer)
    {
        count     = from.count;
        allocated = from.allocated;
        buffer    = from.buffer;
        return;
    }

    reserve(std::max(len + 1, capacity));
    std::copy_n(from.buffer, len + 1, buffer);
    count = len;
}

Utf8::Utf8(Utf8&& from) noexcept
{
    count          = from.count;
    allocated      = from.allocated;
    buffer         = from.buffer;
    from.count     = 0;
    from.allocated = 0;
    from.buffer    = nullptr;
}

void Utf8::reserve(uint32_t newSize)
{
    if (newSize <= allocated)
        return;
    makeLocal();

    const auto lastAllocated = allocated;
    allocated *= 2;
    allocated            = std::max(allocated, newSize);
    const auto newBuffer = Allocator::allocN<char>(allocated);
    if (count)
        std::copy_n(buffer, count + 1, newBuffer);

    Allocator::free(buffer, lastAllocated);

#ifdef SWAG_STATS
    g_Stats.memUtf8 += allocated;
    g_Stats.memUtf8 -= lastAllocated;
#endif

    buffer = newBuffer;
}

bool Utf8::empty() const
{
    return count == 0;
}

uint32_t Utf8::length() const
{
    return count;
}

Utf8 Utf8::toZeroTerminated() const
{
    if (!buffer || !buffer[count])
        return *this;

    Utf8 res;
    res.reserve(count + 1);
    std::copy_n(buffer, count, res.buffer);
    res.buffer[count] = 0;
    res.count         = count;
    return res;
}

const char* Utf8::cstr() const
{
    if (!buffer)
        return "";
    if (!buffer[count])
        return buffer;

    // Big huge leak... not so huge in fact
    // Should limit the call to c_str() as much as possible in a normal run (no errors)
    const auto size = count + 1;
    const auto buf  = Allocator::allocN<char>(size);
    std::copy_n(buffer, count, buf);
    buf[count] = 0;

#ifdef SWAG_STATS
    g_Stats.memUtf8CStr += size;
#endif
    return buf;
}

void Utf8::clear()
{
    count = 0;
}

uint32_t Utf8::capacity() const
{
    return allocated;
}

const char* Utf8::begin() const
{
    return buffer;
}

const char* Utf8::end() const
{
    return buffer + count;
}

Utf8& Utf8::operator=(const char* txt)
{
    clear();
    append(txt);
    return *this;
}

Utf8& Utf8::operator=(Utf8&& other) noexcept
{
    if (&other == this)
        return *this;

    release();
    count           = other.count;
    allocated       = other.allocated;
    buffer          = other.buffer;
    other.count     = 0;
    other.allocated = 0;
    other.buffer    = nullptr;
    return *this;
}

Utf8& Utf8::operator=(const Utf8& other)
{
    if (&other == this)
        return *this;

    // View
    if (!other.allocated && other.buffer)
    {
        freeBuffer();
        count     = other.count;
        allocated = other.allocated;
        buffer    = other.buffer;
    }
    else
    {
        clear();
        append(other);
    }

    return *this;
}

Utf8& Utf8::operator=(uint32_t c)
{
    clear();
    append(c);
    return *this;
}

Utf8 operator+(const Utf8& str1, const char* str2)
{
    Utf8 result{str1};
    result.append(str2);
    return result;
}

void Utf8::operator+=(char c)
{
    append(c);
}

void Utf8::operator+=(const Utf8& txt)
{
    append(txt);
}

void Utf8::operator+=(const char* txt)
{
    append(txt);
}

void Utf8::operator+=(uint32_t c)
{
    if (c <= 0x7F)
    {
        if (count + 2 > allocated)
            reserve(count + 2);
        buffer[count++] = static_cast<char>(c);
        buffer[count]   = 0;
    }
    else
        append(c);
}

Utf8::operator const char*() const
{
    return cstr();
}

Utf8::operator std::string_view() const
{
    return {buffer, count};
}

char Utf8::operator[](uint32_t index) const
{
    SWAG_ASSERT(index <= count);
    return index == count ? static_cast<char>(0) : buffer[index];
}

bool operator<(const Utf8& txt1, const Utf8& txt2)
{
    if (txt1.buffer == txt2.buffer)
        return false;
    if (txt1.count < txt2.count)
        return true;
    if (txt1.count > txt2.count)
        return false;
    return strncmp(txt1.buffer, txt2.buffer, txt1.count) < 0;
}

bool operator==(const Utf8& str1, char c)
{
    if (str1.count != 1)
        return false;
    return str1.buffer[0] == c;
}

bool operator==(const Utf8& str1, const char* str2)
{
    SWAG_ASSERT(str2);
    if (str1.count == 0)
        return false;
    const auto len = static_cast<uint32_t>(strlen(str2));
    if (str1.count != len)
        return false;
    return !strncmp(str1.buffer, str2, str1.count);
}

bool operator==(const Utf8& str1, const Utf8& str2)
{
    if (str1.count != str2.count)
        return false;
    if (str1.count == 0)
        return true;
    return !strncmp(str1.buffer, str2.buffer, str1.count);
}

bool operator!=(const Utf8& str1, const char* str2)
{
    return !(str1 == str2);
}

bool operator!=(const Utf8& str1, const Utf8& str2)
{
    return !(str1 == str2);
}

char Utf8::back() const
{
    SWAG_ASSERT(buffer && count);
    return buffer[count - 1];
}

void Utf8::makeUpper()
{
    makeLocal();
    for (uint32_t i = 0; i < count; i++)
        buffer[i]   = static_cast<char>(toupper(buffer[i]));
}

void Utf8::makeLower()
{
    makeLocal();
    for (uint32_t i = 0; i < count; i++)
        buffer[i]   = static_cast<char>(tolower(buffer[i]));
}

bool Utf8::compareNoCase(const Utf8& txt1) const
{
    if (count != txt1.count)
        return false;
    return _strnicmp(buffer, txt1.buffer, count) == 0;
}

void Utf8::replaceAll(char src, char dst)
{
    makeLocal();
    for (uint32_t i = 0; i < count; i++)
    {
        if (buffer[i] == src)
            buffer[i] = dst;
    }
}

void Utf8::replaceAll(const Utf8& src, const Utf8& dst)
{
    bool reStart = true;
    while (reStart)
    {
        Utf8 result;
        reStart = false;

        uint32_t lastIt = 0;
        auto     it     = find(src);
        while (it != -1)
        {
            reStart = true;
            result += Utf8(buffer + lastIt, it - lastIt);
            result += dst;
            lastIt = it + src.length();
            it     = find(src, lastIt);
        }

        result += Utf8(buffer + lastIt, length() - lastIt);
        *this = result;
    }
}

void Utf8::trimLeft()
{
    if (!count)
        return;

    makeLocal();
    auto pz       = buffer;
    auto newCount = count;
    while (SWAG_IS_BLANK(*pz))
    {
        pz++;
        newCount--;
    }

    memmove(buffer, pz, newCount);
    count         = newCount;
    buffer[count] = 0;
}

void Utf8::trimRight()
{
    if (!count)
        return;

    makeLocal();
    while (count && SWAG_IS_BLANK(buffer[count - 1]))
    {
        count--;
        buffer[count] = 0;
    }
}

void Utf8::trim()
{
    trimLeft();
    trimRight();
}

void Utf8::resize(uint32_t newSize)
{
    reserve(newSize + 1);
    buffer[newSize] = 0;
    count           = newSize;
}

void Utf8::removeBack()
{
    makeLocal();
    SWAG_ASSERT(count);
    count--;
    buffer[count] = 0;
}

void Utf8::trimLeftEol()
{
    trimLeft();
    while (length() && buffer[0] == '\n')
    {
        remove(0, 1);
        trimLeft();
    }
}

void Utf8::trimRightEol()
{
    trimRight();
    while (length() && back() == '\n')
    {
        removeBack();
        trimRight();
    }
}

bool Utf8::containsNoCase(const Utf8& str) const
{
    auto me = *this;
    me.makeLower();
    auto other = str;
    other.makeLower();
    return me.find(other) != -1;
}

int Utf8::find(const Utf8& str, uint32_t startPos) const
{
    if (!count || startPos == count)
        return -1;

    SWAG_ASSERT(startPos < count);
    const auto pz = std::search(buffer + startPos, buffer + count, str.buffer, str.buffer + str.count);
    if (pz == buffer + count)
        return -1;
    return static_cast<int>(pz - buffer);
}

bool Utf8::toChar32(uint32_t& ch) const
{
    ch = 0;
    VectorNative<uint32_t> uni;
    toUni32(uni, 2);
    if (uni.size() != 1)
        return false;
    ch = uni[0];
    return true;
}

std::wstring Utf8::toWString() const
{
    std::string  w8 = cstr();
    std::wstring w{w8.begin(), w8.end()};
    return w;
}

void Utf8::toUni32(VectorNative<uint32_t>& uni, int maxChars) const
{
    uni.clear();

    unsigned    offset;
    const char* start = buffer;
    const auto  end   = buffer + count;
    while (start != end)
    {
        if (maxChars != -1 && std::cmp_greater_equal(uni.size(), maxChars))
            return;
        uint32_t c;
        start = decodeUtf8(start, c, offset);
        uni.push_back(c);
    }
}

void Utf8::toUni16(VectorNative<uint16_t>& uni, int maxChars) const
{
    uni.clear();

    unsigned    offset;
    const char* start = buffer;
    const auto  end   = buffer + count;
    while (start != end)
    {
        if (maxChars != -1 && uni.size() >= static_cast<size_t>(maxChars))
            return;
        uint32_t c;
        start = decodeUtf8(start, c, offset);
        uni.push_back(static_cast<uint16_t>(c));
    }
}

void Utf8::setView(const char* txt, uint32_t len)
{
    release();
    buffer    = const_cast<char*>(txt);
    count     = len;
    allocated = 0;
}

void Utf8::setView(const Utf8& other)
{
    release();
    buffer    = const_cast<char*>(other.buffer);
    count     = other.count;
    allocated = 0;
}

void Utf8::makeLocal()
{
    if (allocated || !buffer || !count)
        return;

    allocated      = count + 1;
    const auto buf = Allocator::allocN<char>(allocated);
    std::copy_n(buffer, count, buf);
    buffer        = buf;
    buffer[count] = 0;

#ifdef SWAG_STATS
    g_Stats.memUtf8 += allocated;
#endif
}

void Utf8::append(const char* txt, uint32_t len)
{
    if (!len)
        return;
    SWAG_ASSERT(txt);
    reserve(count + len + 1);
    std::copy_n(txt, len, buffer + count);
    count += len;
    buffer[count] = 0;
}

void Utf8::append(const char* txt)
{
    if (!txt)
        return;
    const uint32_t len = static_cast<uint32_t>(strlen(txt));
    if (!len)
        return;
    reserve(count + len + 1);
    std::copy_n(txt, len, buffer + count);
    count += len;
    buffer[count] = 0;
}

void Utf8::append(const Utf8& txt)
{
    const auto len = txt.count;
    if (!len)
        return;
    reserve(count + len + 1);
    std::copy_n(txt.buffer, len, buffer + count);
    count += len;
    buffer[count] = 0;
}

void Utf8::append(char c)
{
    reserve(count + 2);
    buffer[count] = c;
    count++;
    buffer[count] = 0;
}

void Utf8::append(uint32_t utf)
{
    if (utf <= 0x7F)
    {
        reserve(count + 2);
        buffer[count++] = static_cast<char>(utf);
    }
    else if (utf <= 0x07FF)
    {
        reserve(count + 3);
        buffer[count++] = static_cast<char>(((utf >> 6) & 0x1F) | 0xC0);
        buffer[count++] = static_cast<char>(((utf >> 0) & 0x3F) | 0x80);
    }
    else if (utf <= 0xFFFF)
    {
        reserve(count + 4);
        buffer[count++] = static_cast<char>(((utf >> 12) & 0x0F) | 0xE0);
        buffer[count++] = static_cast<char>(((utf >> 6) & 0x3F) | 0x80);
        buffer[count++] = static_cast<char>(((utf >> 0) & 0x3F) | 0x80);
    }
    else if (utf <= MAX_ENCODED_UNICODE)
    {
        reserve(count + 5);
        buffer[count++] = static_cast<char>(((utf >> 18) & 0x07) | 0xF0);
        buffer[count++] = static_cast<char>(((utf >> 12) & 0x3F) | 0x80);
        buffer[count++] = static_cast<char>(((utf >> 6) & 0x3F) | 0x80);
        buffer[count++] = static_cast<char>(((utf >> 0) & 0x3F) | 0x80);
    }
    else
    {
        reserve(count + 4);
        buffer[count++] = static_cast<char>(0xEF);
        buffer[count++] = static_cast<char>(0xBF);
        buffer[count++] = static_cast<char>(0xBD);
    }

    buffer[count] = 0;
}

void Utf8::remove(uint32_t index, uint32_t len)
{
    SWAG_ASSERT(index + len <= count);
    makeLocal();
    memmove(buffer + index, buffer + index + len, count - index - len);
    count -= len;
    buffer[count] = 0;
}

void Utf8::insert(uint32_t index, const char* str)
{
    if (index >= count)
    {
        append(str);
        return;
    }

    makeLocal();
    const uint32_t len = static_cast<uint32_t>(strlen(str));
    reserve(count + len + 1);
    memmove(buffer + index + len, buffer + index, count - index);
    std::copy_n(str, len, buffer + index);
    count += len;
    buffer[count] = 0;
}

void Utf8::insert(uint32_t index, char c)
{
    if (index >= count)
    {
        append(c);
        return;
    }

    makeLocal();
    reserve(count + 2);
    memmove(buffer + index + 1, buffer + index, count - index);
    buffer[index] = c;
    count += 1;
    buffer[count] = 0;
}

void Utf8::replace(const char* src, const char* dst, bool wordBoundaryOnly)
{
    makeLocal();
    const uint32_t len    = static_cast<uint32_t>(strlen(src));
    const uint32_t lenIns = static_cast<uint32_t>(strlen(dst));
    uint32_t       pos    = 0;
    while (true)
    {
        const auto nPos = find(src, pos);
        if (nPos == -1)
            break;

        if (wordBoundaryOnly)
        {
            const bool startsAtBoundary = (pos == 0) || !SWAG_IS_ALPHA(buffer[pos - 1]);
            if (!startsAtBoundary)
            {
                pos += 1; // Move forward by 1 to continue searching
                continue;
            }
        }

        pos = static_cast<uint32_t>(nPos);
        remove(pos, len);
        insert(pos, dst);
        pos += lenIns;
    }
}

uint32_t Utf8::hash() const
{
    return Crc32::compute(reinterpret_cast<const uint8_t*>(buffer), count);
}

const char* Utf8::decodeUtf8(const char* pz, uint32_t& wc, unsigned& offset)
{
    const uint8_t c = *pz++;

    if ((c & 0x80) == 0)
    {
        offset = 1;
        wc     = c;
        return pz;
    }

    if ((c & 0xE0) == 0xC0)
    {
        offset = 2;
        wc     = (c & 0x1F) << 6;
        wc |= *pz++ & 0x3F;
        return pz;
    }

    if ((c & 0xF0) == 0xE0)
    {
        offset = 3;
        wc     = (c & 0xF) << 12;
        wc |= (*pz++ & 0x3F) << 6;
        wc |= *pz++ & 0x3F;
        return pz;
    }

    if ((c & 0xF8) == 0xF0)
    {
        offset = 4;
        wc     = (c & 0x7) << 18;
        wc |= (*pz++ & 0x3F) << 12;
        wc |= (*pz++ & 0x3F) << 6;
        wc |= *pz++ & 0x3F;
        return pz;
    }

    if ((c & 0xFC) == 0xF8)
    {
        offset = 1;
        wc     = (c & 0x3) << 24;
        wc |= (c & 0x3F) << 18;
        wc |= (c & 0x3F) << 12;
        wc |= (c & 0x3F) << 6;
        wc |= c & 0x3F;
        return pz;
    }

    if ((c & 0xFE) == 0xFC)
    {
        offset = 1;
        wc     = (c & 0x1) << 30;
        wc |= (c & 0x3F) << 24;
        wc |= (c & 0x3F) << 18;
        wc |= (c & 0x3F) << 12;
        wc |= (c & 0x3F) << 6;
        wc |= c & 0x3F;
        return pz;
    }

    offset = 1;
    wc     = c;
    return pz;
}

void Utf8::tokenize(const Utf8& str, char c, Vector<Utf8>& tokens, bool keepEmpty, bool trim)
{
    tokens.clear();

    auto pz = str.buffer;
    if (!pz)
        return;

    auto i = str.count;
    while (i)
    {
        Utf8 one;
        while (i && *pz != c)
        {
            one += *pz++;
            i--;
        }

        if (trim)
            one.trim();

        if (keepEmpty)
        {
            if (*pz == c)
            {
                pz++;
                i--;
            }
        }
        else
        {
            while (i && *pz == c)
            {
                pz++;
                i--;
            }
        }

        if (!one.empty() || keepEmpty)
            tokens.push_back(one);
    }
}

void Utf8::tokenizeBlanks(const Utf8& str, Vector<Utf8>& tokens)
{
    tokens.clear();

    auto pz = str.buffer;
    if (!pz)
        return;

    auto i = str.count;
    while (i)
    {
        Utf8 one;
        while (i && !SWAG_IS_BLANK(*pz))
        {
            one += *pz++;
            i--;
        }

        while (i && SWAG_IS_BLANK(*pz))
        {
            pz++;
            i--;
        }

        if (!one.empty())
            tokens.push_back(one);
    }
}

Utf8 Utf8::toStringF64(double v)
{
    Utf8 s = form("%.35lf", v);
    while (s.buffer[s.count - 1] == '0')
    {
        s.buffer[s.count - 1] = 0;
        s.count--;
    }

    s += "0";
    return s;
}

Utf8 Utf8::toNiceSize(size_t size)
{
    if (size <= 1)
        return form("%u byte", size);
    if (size < 1024)
        return form("%u bytes", size);
    if (size < static_cast<size_t>(1024) * 1024)
        return form("%.1f Kb", static_cast<float>(size) / 1024.0f);
    if (size < static_cast<size_t>(1024) * 1024 * 1024)
        return form("%.1f Mb", static_cast<float>(size) / (1024.0f * 1024.0f));
    return form("%.1f Gb", static_cast<float>(size) / (1024.0f * 1024.0f * 1024.0f));
}

uint32_t Utf8::fuzzyCompare(const Utf8& str1, const Utf8& str2)
{
    if (str1 == str2)
        return 0;

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

    unsigned int       y, lastDiag;
    const unsigned int s1Len = str1.length();
    const unsigned int s2Len = str2.length();

    const auto column = new unsigned int[s1Len + 1];
    for (y        = 1; y <= s1Len; y++)
        column[y] = y;
    for (unsigned int x = 1; x <= s2Len; x++)
    {
        column[0] = x;
        for (y = 1, lastDiag = x - 1; y <= s1Len; y++)
        {
            const unsigned int oldDiag = column[y];
            column[y]                  = MIN3(column[y] + 1, column[y - 1] + 1, lastDiag + (str1[y - 1] == str2[x - 1] ? 0 : 1));
            lastDiag                   = oldDiag;
        }
    }

    const auto result = column[s1Len];
    return result;
}

Utf8 Utf8::truncateDisplay(const char* str, int maxLen)
{
    Utf8 result;
    int  cpt = 0;
    auto pz  = str;
    while (pz && *pz && cpt++ < maxLen)
        result += *pz++;
    if (*pz)
        result += " ...";
    return result;
}

bool Utf8::isNumber(const char* pz)
{
    while (*pz)
    {
        if (!isdigit(*pz))
            return false;
        pz++;
    }

    return true;
}

bool Utf8::startsWith(const char* pz) const
{
    const auto len = strlen(pz);
    if (len > count)
        return false;
    return memcmp(buffer, pz, len) == 0;
}

int Utf8::countOf(char c) const
{
    int cpt = 0;
    for (uint32_t i = 0; i < count; i++)
    {
        if (buffer[i] == c)
            cpt++;
    }
    return cpt;
}

int Utf8::toInt(uint32_t offset) const
{
    SWAG_ASSERT(offset < length());
    char* end;
    return strtol(buffer + offset, &end, 10);
}

uint32_t Utf8::countStartBlanks() const
{
    if (!length())
        return 0;

    auto     pz          = buffer;
    uint32_t countBlanks = 0;
    while (SWAG_IS_BLANK(*pz))
    {
        pz++;
        countBlanks++;
    }

    return countBlanks;
}

Utf8 form(const char* format, va_list args)
{
    va_list argsCopy;
    va_copy(argsCopy, args);
    const size_t len = vsnprintf(nullptr, 0, format, argsCopy); // NOLINT(clang-diagnostic-format-nonliteral)
    va_end(argsCopy);

    Utf8 vec;
    vec.resize(static_cast<uint32_t>(len));

    (void) vsnprintf(vec.buffer, len + 1, format, args); // NOLINT(clang-diagnostic-format-nonliteral)
    return vec;
}

Utf8 form(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    auto result = form(format, args);
    va_end(args);
    return result;
}

// Turn any multi-line signature into a single, space-collapsed line.
Utf8 Utf8::oneLine(const Utf8& in)
{
    Utf8 out = in;

    // Replace newlines/tabs with spaces
    out.replaceAll('\n', ' ');
    out.replaceAll('\r', ' ');
    out.replaceAll('\t', ' ');

    // Collapse multiple spaces
    Utf8 collapsed;
    collapsed.reserve(out.length());

    bool prevSpace = false;
    for (uint32_t i = 0; i < out.length(); i++)
    {
        const char c       = out[i];
        const bool isSpace = (c == ' ');
        if (isSpace && prevSpace)
            continue;

        collapsed.append(c);
        prevSpace = isSpace;
    }

    collapsed.trim(); // trim leading/trailing spaces
    return collapsed;
}

// Middle-ellipsize to fit maxWidth code units (bytes). Keeps both ends visible.
Utf8 Utf8::ellipsizeMiddle(const Utf8& in, uint32_t maxWidth)
{
    const uint32_t n = in.length();
    if (n <= maxWidth || maxWidth < 4)
        return in;

    static constexpr auto ELLIPSIS = " ... ";
    const auto            ellLen   = static_cast<uint32_t>(strlen(ELLIPSIS));

    const uint32_t keep  = (maxWidth > ellLen) ? (maxWidth - ellLen) : 0;
    const uint32_t left  = keep / 2;
    const uint32_t right = keep - left;

    Utf8 out;
    out.reserve(maxWidth);

    // Left part
    if (left)
        out.append(in.begin(), left);

    // Ellipsis
    out.append(ELLIPSIS, ellLen);

    // Right part
    if (right)
        out.append(in.begin() + (n - right), right);

    return out;
}

Utf8 Utf8::substr(uint32_t start, uint32_t len) const
{
    // Guard: empty string or start beyond end
    if (start >= length())
        return {};

    const char* begin = cstr();
    const char* end   = begin + length();

    // Move to the starting byte position, respecting UTF-8 boundaries
    const char* p = begin;
    size_t bytePos = 0;
    while (p < end && bytePos < start)
    {
        const unsigned char c = static_cast<unsigned char>(*p);
        size_t charLen  = 1;
        if      ((c & 0x80) == 0x00) charLen = 1;
        else if ((c & 0xE0) == 0xC0) charLen = 2;
        else if ((c & 0xF0) == 0xE0) charLen = 3;
        else if ((c & 0xF8) == 0xF0) charLen = 4;
        p += charLen;
        bytePos++;
    }

    const char* startPtr = p;
    const char* cur      = startPtr;

    size_t byteCount = 0;
    while (cur < end && (len == UINT32_MAX || byteCount < len))
    {
        const unsigned char c = static_cast<unsigned char>(*cur);
        size_t charLen  = 1;
        if      ((c & 0x80) == 0x00) charLen = 1;
        else if ((c & 0xE0) == 0xC0) charLen = 2;
        else if ((c & 0xF0) == 0xE0) charLen = 3;
        else if ((c & 0xF8) == 0xF0) charLen = 4;
        cur += charLen;
        byteCount++;
    }

    return Utf8{startPtr, static_cast<unsigned int>(cur - startPtr)};
}