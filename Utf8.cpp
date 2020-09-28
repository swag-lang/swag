#include "pch.h"
#include "Utf8.h"
#include "Allocator.h"
#include "Runtime.h"
#include "swag_runtime.h"

void Utf8::reserve(int newSize)
{
    if (newSize <= allocated)
        return;

    // No need to to something, because we are sure to have at least UTF8_SMALL_SIZE
    // available room
    if (newSize <= UTF8_SMALL_SIZE)
        return;

    auto lastAllocated = allocated;
    allocated *= 2;
    allocated      = max(allocated, newSize);
    allocated      = g_Allocator.alignSize(allocated);
    auto newBuffer = (char*) g_Allocator.alloc(allocated);
    if (count)
        swag_runtime_memcpy(newBuffer, buffer, count + 1);

    if (lastAllocated != UTF8_SMALL_SIZE)
        g_Allocator.free(buffer, lastAllocated);
    buffer = newBuffer;
}

Utf8::Utf8()
{
    buffer    = padding;
    count     = 0;
    allocated = UTF8_SMALL_SIZE;
    buffer[0] = 0;
}

Utf8::~Utf8()
{
    if (allocated > UTF8_SMALL_SIZE)
        g_Allocator.free(buffer, allocated);
}

void Utf8::reset()
{
    if (allocated > UTF8_SMALL_SIZE)
        g_Allocator.free(buffer, allocated);
    buffer    = padding;
    count     = 0;
    allocated = UTF8_SMALL_SIZE;
    buffer[0] = 0;
}

Utf8::Utf8(const char* from)
{
    buffer    = padding;
    count     = 0;
    allocated = UTF8_SMALL_SIZE;
    buffer[0] = 0;

    int len = from ? (int) strlen(from) : 0;
    if (!len)
        return;

    reserve(len + 1);
    swag_runtime_memcpy(buffer, from, len + 1);
    count = len;
}

Utf8::Utf8(const string& from)
{
    buffer    = padding;
    count     = 0;
    allocated = UTF8_SMALL_SIZE;
    buffer[0] = 0;

    int len = (int) from.length();
    if (!len)
        return;

    reserve(len + 1);
    swag_runtime_memcpy(buffer, from.c_str(), len + 1);
    count = len;
}

Utf8::Utf8(const Utf8& from)
{
    buffer    = padding;
    count     = 0;
    allocated = UTF8_SMALL_SIZE;
    buffer[0] = 0;

    int len = from.count;
    if (!len)
        return;

    reserve(len + 1);
    swag_runtime_memcpy(buffer, from.buffer, len + 1);
    count = len;
}

Utf8::Utf8(Utf8&& from)
{
    count     = from.count;
    allocated = from.allocated;

    if (from.allocated == UTF8_SMALL_SIZE)
    {
        static_assert(UTF8_SMALL_SIZE == 8);
        buffer              = padding;
        *(uint64_t*) buffer = *(uint64_t*) from.buffer;
    }
    else
    {
        buffer      = from.buffer;
        from.buffer = nullptr;
        from.reset();
    }
}

void Utf8::operator=(Utf8&& from)
{
    reset();
    count     = from.count;
    allocated = from.allocated;

    if (from.allocated == UTF8_SMALL_SIZE)
    {
        static_assert(UTF8_SMALL_SIZE == 8);
        buffer              = padding;
        *(uint64_t*) buffer = *(uint64_t*) from.buffer;
    }
    else
    {
        buffer      = from.buffer;
        from.buffer = nullptr;
        from.reset();
    }
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

void Utf8::append(const char* txt, int len)
{
    if (!len)
        return;
    SWAG_ASSERT(txt);
    reserve(count + len + 1);
    swag_runtime_memcpy(buffer + count, txt, len + 1);
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
    swag_runtime_memcpy(buffer + count, txt, len + 1);
    count += len;
}

void Utf8::append(const Utf8& txt)
{
    int len = txt.count;
    if (!len)
        return;
    reserve(count + len + 1);
    swag_runtime_memcpy(buffer + count, txt.buffer, len + 1);
    count += len;
}

void Utf8::append(char c)
{
    reserve(count + 2);
    buffer[count]     = c;
    buffer[count + 1] = 0;
    count++;
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

void Utf8::operator+=(const Utf8& txt)
{
    append(txt);
}

void Utf8::operator+=(const char* txt)
{
    append(txt);
}

void Utf8::operator=(const Utf8& other)
{
    clear();
    append(other);
}

Utf8::operator const char*()
{
    return buffer;
}

char Utf8::operator[](int index) const
{
    return index < count ? buffer[index] : 0;
}

Utf8 operator+(const Utf8& str1, const char* str2)
{
    Utf8 result{str1};
    result.append(str2);
    return move(result);
}

Utf8 operator+(const char* str1, const Utf8& str2)
{
    Utf8 result{str1};
    result.append(str2);
    return move(result);
}

bool operator==(const Utf8& str1, const char* str2)
{
    SWAG_ASSERT(str2);
    if (str1.count == 0)
        return false;
    return !strcmp(str1.buffer, str2);
}

bool operator!=(const Utf8& str1, const char* str2)
{
    SWAG_ASSERT(str2);
    if (str1.count == 0)
        return true;
    return strcmp(str1.buffer, str2) != 0;
}

bool operator==(const Utf8& str1, const Utf8& str2)
{
    if (str1.count != str2.count)
        return false;
    if (str1.count == 0)
        return true;
    return !strcmp(str1.buffer, str2.buffer);
}

bool operator!=(const Utf8& str1, const Utf8& str2)
{
    if (str1.count != str2.count)
        return true;
    if (str1.count == 0)
        return false;
    return strcmp(str1.buffer, str2.buffer) != 0;
}

void Utf8::operator=(char32_t c)
{
    clear();
    append(c);
}

void Utf8::operator+=(char32_t c)
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

char Utf8::back() const
{
    SWAG_ASSERT(buffer && count);
    return buffer[count - 1];
}

void Utf8::makeUpper()
{
    for (int i = 0; i < count; i++)
    {
        buffer[i] = (char) toupper(buffer[i]);
    }
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
    while (UTF8_IS_BLANK(*pz))
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
    while (count && UTF8_IS_BLANK(buffer[count - 1]))
        count--;
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

int Utf8::find(const char* str) const
{
    if (!count)
        return -1;
    auto pz = strstr(buffer, str);
    if (!pz)
        return -1;
    return (int) (pz - buffer);
}

bool Utf8::toChar32(char32_t& ch)
{
    ch = 0;
    VectorNative<char32_t> uni;
    toUni32(uni, 2);
    if (uni.size() != 1)
        return false;
    ch = uni[0];
    return true;
}

const char* Utf8::transformUtf8ToChar32(const char* pz, char32_t& wc)
{
    auto c = *pz++;

    if ((c & 0x80) == 0)
    {
        wc = c;
        return pz;
    }

    if ((c & 0xE0) == 0xC0)
    {
        wc = (c & 0x1F) << 6;
        wc |= (*pz++ & 0x3F);
        return pz;
    }

    if ((c & 0xF0) == 0xE0)
    {
        wc = (c & 0xF) << 12;
        wc |= (*pz++ & 0x3F) << 6;
        wc |= (*pz++ & 0x3F);
        return pz;
    }

    if ((c & 0xF8) == 0xF0)
    {
        wc = (c & 0x7) << 18;
        wc |= (*pz++ & 0x3F) << 12;
        wc |= (*pz++ & 0x3F) << 6;
        wc |= (*pz++ & 0x3F);
        return pz;
    }

    if ((c & 0xFC) == 0xF8)
    {
        wc = (c & 0x3) << 24;
        wc |= (c & 0x3F) << 18;
        wc |= (c & 0x3F) << 12;
        wc |= (c & 0x3F) << 6;
        wc |= (c & 0x3F);
        return pz;
    }

    if ((c & 0xFE) == 0xFC)
    {
        wc = (c & 0x1) << 30;
        wc |= (c & 0x3F) << 24;
        wc |= (c & 0x3F) << 18;
        wc |= (c & 0x3F) << 12;
        wc |= (c & 0x3F) << 6;
        wc |= (c & 0x3F);
        return pz;
    }

    wc = c;
    return pz;
}

void Utf8::toUni32(VectorNative<char32_t>& uni, int maxChars)
{
    uni.clear();
    const char* pz = buffer;
    while (*pz)
    {
        if (maxChars != -1 && uni.size() >= maxChars)
            return;
        char32_t c;
        pz = transformUtf8ToChar32(pz, c);
        uni.push_back(c);
    }
}

void Utf8::toUni16(VectorNative<char16_t>& uni, int maxChars)
{
    uni.clear();
    const char* pz = buffer;
    while (*pz)
    {
        if (maxChars != -1 && uni.size() >= maxChars)
            return;
        char32_t c;
        pz = transformUtf8ToChar32(pz, c);
        uni.push_back((char16_t) c);
    }
}

void Utf8::append(char32_t utf)
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