#include "pch.h"
#include "Utf8.h"
#include "CommandLine.h"

Utf8::Utf8()
{
}

Utf8::~Utf8()
{
    g_Allocator.free(buffer, allocated);
}

void Utf8::reset()
{
    g_Allocator.free(buffer, allocated);
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

    reserve(len + 1);
    memcpy(buffer, from.buffer, len + 1);
    count = len;
}

Utf8::Utf8(Utf8&& from)
{
    count       = from.count;
    allocated   = from.allocated;
    buffer      = from.buffer;
    from.buffer = nullptr;
    from.reset();
}

void Utf8::reserve(int newSize)
{
    if (newSize <= allocated)
        return;

    auto lastAllocated = allocated;
    allocated *= 2;
    allocated      = max(allocated, newSize);
    allocated      = (int) Allocator::alignSize(allocated);
    auto newBuffer = (char*) g_Allocator.alloc(allocated);
    if (count)
        memcpy(newBuffer, buffer, count + 1);
    if (g_CommandLine->stats)
        g_Stats.memUtf8 += allocated;

    g_Allocator.free(buffer, lastAllocated);
    if (g_CommandLine->stats)
        g_Stats.memUtf8 -= lastAllocated;

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

const char* Utf8::c_str() const
{
    static const char* nullString = "";
    return count ? buffer : nullString;
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
    reset();
    count       = from.count;
    allocated   = from.allocated;
    buffer      = from.buffer;
    from.buffer = nullptr;
    from.reset();
}

void Utf8::operator=(const Utf8& other)
{
    if (&other == this)
        return;
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
    return buffer;
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
    return !strcmp(str1.buffer, str2);
}

bool operator==(const Utf8& str1, const Utf8& str2)
{
    if (str1.count != str2.count)
        return false;
    if (str1.count == 0)
        return true;
    return !strcmp(str1.buffer, str2.buffer);
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
    for (int i = 0; i < count; i++)
        buffer[i] = (char) toupper(buffer[i]);
}

bool Utf8::compareNoCase(const Utf8& txt1)
{
    if (count != txt1.count)
        return false;
    return _strcmpi(buffer, txt1.buffer) == 0;
}

void Utf8::replaceAll(char src, char dst)
{
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

void Utf8::pop_back()
{
    SWAG_ASSERT(count);
    count--;
    buffer[count] = 0;
}

int Utf8::find(const char* str, int startpos) const
{
    if (!count)
        return -1;
    auto pz = strstr(buffer + startpos, str);
    if (!pz)
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
    const char* pz = buffer;
    while (*pz)
    {
        if (maxChars != -1 && uni.size() >= maxChars)
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
    const char* pz = buffer;
    while (*pz)
    {
        if (maxChars != -1 && uni.size() >= maxChars)
            return;
        uint32_t c;
        pz = decodeUtf8(pz, c, offset);
        uni.push_back((uint16_t) c);
    }
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
        buffer[count++] = (uint8_t)(((utf >> 6) & 0x1F) | 0xC0);
        buffer[count++] = (uint8_t)(((utf >> 0) & 0x3F) | 0x80);
    }
    else if (utf <= 0xFFFF)
    {
        reserve(count + 4);
        buffer[count++] = (uint8_t)(((utf >> 12) & 0x0F) | 0xE0);
        buffer[count++] = (uint8_t)(((utf >> 6) & 0x3F) | 0x80);
        buffer[count++] = (uint8_t)(((utf >> 0) & 0x3F) | 0x80);
    }
    else if (utf <= 0x10FFFF)
    {
        reserve(count + 5);
        buffer[count++] = (uint8_t)(((utf >> 18) & 0x07) | 0xF0);
        buffer[count++] = (uint8_t)(((utf >> 12) & 0x3F) | 0x80);
        buffer[count++] = (uint8_t)(((utf >> 6) & 0x3F) | 0x80);
        buffer[count++] = (uint8_t)(((utf >> 0) & 0x3F) | 0x80);
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

    int len = (int) strlen(str);
    reserve(count + len + 1);
    memmove(buffer + index + len, buffer + index, count - index);
    memcpy(buffer + index, str, len);
    count += len;
    buffer[count] = 0;
}

void Utf8::replace(const char* src, const char* dst)
{
    int pos;
    int len, lenins;

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
    return hash(buffer, count);
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

uint32_t Utf8::hash(const char* buffer, int count)
{
    uint32_t hash = 0;

    auto s = buffer;
    while (count > 4)
    {
        hash += *(uint32_t*) s;
        hash += (hash << 10);
        hash ^= (hash >> 6);
        s += 4;
        count -= 4;
    }

    while (count--)
    {
        hash += *s++;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
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

void Utf8::tokenize(const char* str, char c, vector<Utf8>& tokens)
{
    tokens.clear();

    auto pz = str;
    if (!pz)
        return;

    while (*pz)
    {
        Utf8 one;
        while (*pz && *pz != c)
            one += *pz++;
        while (*pz && *pz == c)
            pz++;
        if (!one.empty())
            tokens.push_back(one);
    }
}

void Utf8::tokenizeBlanks(const char* str, vector<Utf8>& tokens)
{
    tokens.clear();

    auto pz = str;
    if (!pz)
        return;

    while (*pz)
    {
        Utf8 one;
        while (*pz && !SWAG_IS_BLANK(*pz))
            one += *pz++;

        if (*pz)
        {
            while (*pz && SWAG_IS_BLANK(*pz))
                pz++;
        }

        if (!one.empty())
            tokens.push_back(one);
    }
}

string Utf8::normalizePath(const fs::path& path)
{
    string str;
    auto   source      = path.string();
    auto   len         = source.length();
    bool   lastIsSlash = false;
    for (int i = 0; i < len; i++)
    {
        char c = source[i];
        if (c == '\\' || c == '/')
        {
            if (lastIsSlash)
                continue;
            lastIsSlash = true;
            str += '/';
            continue;
        }
        else if (c <= '0x7F' && c > 32)
        {
            lastIsSlash = false;
            str += (char) tolower((int) c);
        }
        else
        {
            lastIsSlash = false;
            str += c;
        }
    }

    return str;
}

Utf8 Utf8::toStringF64(double v)
{
    Utf8 s = Utf8::format("%.35lf", v);
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
    if (size < 1024)
        return Utf8::format("%u bytes", size);
    if (size < 1024 * 1024)
        return Utf8::format("%.1f Kb", size / 1024.0f);
    if (size < 1024 * 1024 * 1024)
        return Utf8::format("%.1f Mb", size / (1024.0f * 1024.0f));
    return Utf8::format("%.1f Gb", size / (1024.0f * 1024.0f * 1024.0f));
}

uint32_t Utf8::fuzzyCompare(const Utf8& str1, const Utf8& str2)
{
    int32_t  i, j, diagonal;
    uint32_t cost = 0;

    auto s1len = str1.length();
    auto s2len = str2.length();

    if (s1len + s2len == 0)
        return UINT32_MAX;
    if (s1len * s2len == 0)
        return UINT32_MAX;

    j              = s1len + 1;
    auto allocSize = Allocator::alignSize(j * sizeof(uint32_t));
    auto arr       = (uint32_t*) g_Allocator.alloc(allocSize);

    for (i = 0; i < j; i++)
        arr[i] = i + 1;

    for (i = 0; i < s2len; i++)
    {
        diagonal = arr[0] - 1;
        arr[0]   = i + 1;
        j        = 0;
        while (j < s1len)
        {
            cost = diagonal;
            if (str1[j] != str2[i])
                cost++;
            if (cost > arr[j])
                cost = arr[j];
            j++;
            if (cost > arr[j])
                cost = arr[j];
            diagonal = arr[j] - 1;
            arr[j]   = cost + 1;
        }
    }

    cost = arr[j] - 1;
    g_Allocator.free(arr, allocSize);

    return cost; // (s1len + s2len - cost) / (float)(s1len + s2len);
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