#include "pch.h"
#include "Utf8.h"
#include "Crc32.h"
#include "Statistics.h"

Utf8::Utf8()
{
}

Utf8::~Utf8()
{
    freeBuffer();
}

void Utf8::freeBuffer()
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
    int len = from ? (int) strlen(from) : 0;
    if (!len)
        return;

    reserve(len + 1);
    memcpy(buffer, from, len + 1);
    count = len;
}

Utf8::Utf8(const char* from, uint32_t len)
{
    if (!len)
        return;

    reserve(len + 1);
    memcpy(buffer, from, len + 1);
    count = len;
}

Utf8::Utf8(const string& from)
{
    int len = (int) from.length();
    if (!len)
        return;

    reserve(len + 1);
    memcpy(buffer, from.c_str(), len + 1);
    count = len;
}

Utf8::Utf8(const Utf8& from)
{
    int len = from.count;
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
    memcpy(buffer, from.buffer, len + 1);
    count = len;
}

Utf8::Utf8(const Utf8& from, int capcity)
{
    int len = from.count;
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

    reserve(max(len + 1, capcity));
    memcpy(buffer, from.buffer, len + 1);
    count = len;
}

Utf8::Utf8(Utf8&& from)
{
    count          = from.count;
    allocated      = from.allocated;
    buffer         = from.buffer;
    from.count     = 0;
    from.allocated = 0;
    from.buffer    = nullptr;
}

void Utf8::reserve(int newSize)
{
    if (newSize <= allocated)
        return;
    makeLocal();

    auto lastAllocated = allocated;
    allocated *= 2;
    allocated      = max(allocated, newSize);
    allocated      = (int) Allocator::alignSize(allocated);
    auto newBuffer = (char*) Allocator::alloc(allocated);
    if (count)
        memcpy(newBuffer, buffer, count + 1);

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

int Utf8::length() const
{
    return count;
}

Utf8 Utf8::toZeroTerminated() const
{
    if (!buffer || !buffer[count])
        return *this;

    Utf8 res;
    res.reserve(count + 1);
    memcpy(res.buffer, buffer, count);
    res.buffer[count] = 0;
    res.count         = count;
    return res;
}

const char* Utf8::c_str() const
{
    if (!buffer)
        return "";
    if (!buffer[count])
        return buffer;

    // Big huge leak... not so huge in fact
    // Should limit the call to c_str() as much as possible in a normal run (no errors)
    auto size = (int) Allocator::alignSize(count + 1);
    auto buf  = (char*) Allocator::alloc(size);
    memcpy(buf, buffer, count);
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

int Utf8::capacity() const
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

void Utf8::operator=(const char* txt)
{
    clear();
    append(txt);
}

void Utf8::operator=(Utf8&& from)
{
    release();
    count          = from.count;
    allocated      = from.allocated;
    buffer         = from.buffer;
    from.count     = 0;
    from.allocated = 0;
    from.buffer    = nullptr;
}

void Utf8::operator=(const Utf8& other)
{
    if (&other == this)
        return;

    // View
    if (!other.allocated && other.buffer)
    {
        freeBuffer();
        count     = other.count;
        allocated = other.allocated;
        buffer    = other.buffer;
        return;
    }

    clear();
    append(other);
}

void Utf8::operator=(uint32_t c)
{
    clear();
    append(c);
}

Utf8 operator+(const Utf8& str1, const char* str2)
{
    Utf8 result{str1};
    result.append(str2);
    return result;
}

Utf8 operator+(const char* str1, const Utf8& str2)
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
        buffer[count++] = (uint8_t) c;
        buffer[count]   = 0;
    }
    else
        append(c);
}

Utf8::operator const char*()
{
    return c_str();
}

char Utf8::operator[](int index) const
{
    SWAG_ASSERT(index <= count);
    return index == count ? 0 : buffer[index];
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
    auto len = (uint32_t) strlen(str2);
    if (str1.count != (int) len)
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
    for (int i = 0; i < count; i++)
        buffer[i] = (char) toupper(buffer[i]);
}

void Utf8::makeLower()
{
    makeLocal();
    for (int i = 0; i < count; i++)
        buffer[i] = (char) tolower(buffer[i]);
}

bool Utf8::compareNoCase(const Utf8& txt1)
{
    if (count != txt1.count)
        return false;
    return _strnicmp(buffer, txt1.buffer, count) == 0;
}

void Utf8::replaceAll(char src, char dst)
{
    makeLocal();
    for (int i = 0; i < count; i++)
    {
        if (buffer[i] == src)
            buffer[i] = dst;
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

void Utf8::resize(int newSize)
{
    reserve(newSize + 1);
    buffer[newSize] = 0;
    count           = newSize;
}

void Utf8::removeBack()
{
    SWAG_ASSERT(count);
    count--;
    buffer[count] = 0;
}

int Utf8::find(const Utf8& str, int startpos) const
{
    if (!count)
        return -1;

    auto pz = std::search(buffer, buffer + count, str.buffer, str.buffer + str.count);
    if (pz == buffer + count)
        return -1;
    return (int) (pz - buffer);
}

bool Utf8::toChar32(uint32_t& ch)
{
    ch = 0;
    VectorNative<uint32_t> uni;
    toUni32(uni, 2);
    if (uni.size() != 1)
        return false;
    ch = uni[0];
    return true;
}

void Utf8::toUni32(VectorNative<uint32_t>& uni, int maxChars)
{
    uni.clear();

    unsigned    offset;
    const char* pz    = buffer;
    auto        endpz = buffer + count;
    while (pz != endpz)
    {
        if (maxChars != -1 && uni.size() >= (size_t) maxChars)
            return;
        uint32_t c;
        pz = decodeUtf8(pz, c, offset);
        uni.push_back(c);
    }
}

void Utf8::toUni16(VectorNative<uint16_t>& uni, int maxChars)
{
    uni.clear();

    unsigned    offset;
    const char* pz    = buffer;
    auto        endpz = buffer + count;
    while (pz != endpz)
    {
        if (maxChars != -1 && uni.size() >= (size_t) maxChars)
            return;
        uint32_t c;
        pz = decodeUtf8(pz, c, offset);
        uni.push_back((uint16_t) c);
    }
}

void Utf8::setView(const char* txt, int len)
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

    allocated = (int) Allocator::alignSize(count + 1);
    auto buf  = (char*) Allocator::alloc(allocated);
    memcpy(buf, buffer, count);
    buffer        = buf;
    buffer[count] = 0;

#ifdef SWAG_STATS
    g_Stats.memUtf8 += allocated;
#endif
}

void Utf8::append(const char* txt, int len)
{
    if (!len)
        return;
    SWAG_ASSERT(txt);
    reserve(count + len + 1);
    memcpy(buffer + count, txt, len);
    count += len;
    buffer[count] = 0;
}

void Utf8::append(const char* txt)
{
    if (!txt)
        return;
    int len = (int) strlen(txt);
    if (!len)
        return;
    reserve(count + len + 1);
    memcpy(buffer + count, txt, len);
    count += len;
    buffer[count] = 0;
}

void Utf8::append(const Utf8& txt)
{
    int len = txt.count;
    if (!len)
        return;
    reserve(count + len + 1);
    memcpy(buffer + count, txt.buffer, len);
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
        buffer[count++] = (uint8_t) utf;
    }
    else if (utf <= 0x07FF)
    {
        reserve(count + 3);
        buffer[count++] = (uint8_t) (((utf >> 6) & 0x1F) | 0xC0);
        buffer[count++] = (uint8_t) (((utf >> 0) & 0x3F) | 0x80);
    }
    else if (utf <= 0xFFFF)
    {
        reserve(count + 4);
        buffer[count++] = (uint8_t) (((utf >> 12) & 0x0F) | 0xE0);
        buffer[count++] = (uint8_t) (((utf >> 6) & 0x3F) | 0x80);
        buffer[count++] = (uint8_t) (((utf >> 0) & 0x3F) | 0x80);
    }
    else if (utf <= 0x10FFFF)
    {
        reserve(count + 5);
        buffer[count++] = (uint8_t) (((utf >> 18) & 0x07) | 0xF0);
        buffer[count++] = (uint8_t) (((utf >> 12) & 0x3F) | 0x80);
        buffer[count++] = (uint8_t) (((utf >> 6) & 0x3F) | 0x80);
        buffer[count++] = (uint8_t) (((utf >> 0) & 0x3F) | 0x80);
    }
    else
    {
        reserve(count + 4);
        buffer[count++] = (uint8_t) 0xEF;
        buffer[count++] = (uint8_t) 0xBF;
        buffer[count++] = (uint8_t) 0xBD;
    }

    buffer[count] = 0;
}

void Utf8::remove(int index, int len)
{
    SWAG_ASSERT(index + len <= count);
    makeLocal();
    memmove(buffer + index, buffer + index + len, count - index);
    count -= len;
    buffer[count] = 0;
}

void Utf8::insert(int index, const char* str)
{
    if (index >= count)
    {
        append(str);
        return;
    }

    makeLocal();
    int len = (int) strlen(str);
    reserve(count + len + 1);
    memmove(buffer + index + len, buffer + index, count - index);
    memcpy(buffer + index, str, len);
    count += len;
    buffer[count] = 0;
}

void Utf8::insert(int index, char c)
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

void Utf8::replace(const char* src, const char* dst)
{
    int pos;
    int len, lenins;

    makeLocal();
    len    = (int) strlen(src);
    lenins = (int) strlen(dst);
    pos    = 0;
    do
    {
        pos = find(src, pos);
        if (pos != -1)
        {
            remove(pos, len);
            insert(pos, dst);
            pos += lenins;
        }
    } while (pos != -1);
}

uint32_t Utf8::hash() const
{
    return Crc32::compute((const uint8_t*) buffer, count);
}

const char* Utf8::decodeUtf8(const char* pz, uint32_t& wc, unsigned& offset)
{
    uint8_t c = *pz++;

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
        wc |= (*pz++ & 0x3F);
        return pz;
    }

    if ((c & 0xF0) == 0xE0)
    {
        offset = 3;
        wc     = (c & 0xF) << 12;
        wc |= (*pz++ & 0x3F) << 6;
        wc |= (*pz++ & 0x3F);
        return pz;
    }

    if ((c & 0xF8) == 0xF0)
    {
        offset = 4;
        wc     = (c & 0x7) << 18;
        wc |= (*pz++ & 0x3F) << 12;
        wc |= (*pz++ & 0x3F) << 6;
        wc |= (*pz++ & 0x3F);
        return pz;
    }

    if ((c & 0xFC) == 0xF8)
    {
        offset = 1;
        wc     = (c & 0x3) << 24;
        wc |= (c & 0x3F) << 18;
        wc |= (c & 0x3F) << 12;
        wc |= (c & 0x3F) << 6;
        wc |= (c & 0x3F);
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
        wc |= (c & 0x3F);
        return pz;
    }

    offset = 1;
    wc     = c;
    return pz;
}

Utf8 Utf8::format(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    size_t len = vsnprintf(nullptr, 0, format, args);
    va_end(args);

    Utf8 vec;
    vec.resize((int) len);
    va_start(args, format);
    vsnprintf(vec.buffer, len + 1, format, args);
    va_end(args);
    return vec;
}

void Utf8::tokenize(const Utf8& str, char c, Vector<Utf8>& tokens)
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

        while (i && *pz == c)
        {
            pz++;
            i--;
        }

        if (!one.empty())
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
    Utf8 s = Fmt("%.35lf", v);
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
        return Fmt("%u byte", size);
    if (size < 1024)
        return Fmt("%u bytes", size);
    if (size < 1024 * 1024)
        return Fmt("%.1f Kb", size / 1024.0f);
    if (size < 1024 * 1024 * 1024)
        return Fmt("%.1f Mb", size / (1024.0f * 1024.0f));
    return Fmt("%.1f Gb", size / (1024.0f * 1024.0f * 1024.0f));
}

uint32_t Utf8::fuzzyCompare(const Utf8& str1, const Utf8& str2)
{
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = str1.length();
    s2len = str2.length();

    unsigned int* column = new unsigned int[s1len + 1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++)
    {
        column[0] = x;
        for (y = 1, lastdiag = x - 1; y <= s1len; y++)
        {
            olddiag   = column[y];
            column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (str1[y - 1] == str2[x - 1] ? 0 : 1));
            lastdiag  = olddiag;
        }
    }

    auto result = column[s1len];
    return result;
}

Utf8 Utf8::getExtension(const Utf8& name)
{
    if (!name.buffer)
        return "";

    auto pz = name.buffer + name.count - 1;
    while (pz != name.buffer && *pz != '.')
        pz--;
    if (*pz != '.')
        return "";

    Utf8 result = pz;
    result.makeLower();
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
        result += "...";
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