#pragma once
#include "Assert.h"
extern wstring_convert<codecvt_utf8<int32_t>, int32_t> toUtf8;
#define UTF8_IS_BLANK(__c) (__c == ' ' || __c == '\t' || __c == '\v' || __c == '\f' || __c == '\r')

struct Utf8
{
    char* buffer    = nullptr;
    int   count     = 0;
    int   allocated = 0;

    void reserve(int newSize)
    {
        if (newSize <= allocated)
            return;

        allocated *= 2;
        allocated      = max(allocated, newSize);
        auto newBuffer = (char*) malloc(allocated);
        if (count)
            memcpy(newBuffer, buffer, count + 1);
        free(buffer);
        buffer = newBuffer;
    }

    ~Utf8()
    {
        free(buffer);
    }

    Utf8()
    {
    }

    Utf8(const char* from)
    {
        int len = from ? (int) strlen(from) : 0;
        if (!len)
        {
            buffer = nullptr;
            count = allocated = 0;
            return;
        }

        reserve(len + 1);
        memcpy(buffer, from, len + 1);
        count = len;
    }

    Utf8(const string& from)
    {
        int len = (int) from.length();
        if (!len)
        {
            buffer = nullptr;
            count = allocated = 0;
            return;
        }

        reserve(len + 1);
        memcpy(buffer, from.c_str(), len + 1);
        count = len;
    }

    Utf8(const Utf8& from)
    {
        int len = from.count;
        if (!len)
        {
            buffer = nullptr;
            count = allocated = 0;
            return;
        }

        reserve(len + 1);
        memcpy(buffer, from.buffer, len + 1);
        count = len;
    }

    bool empty() const
    {
        return count == 0;
    }

    int length() const
    {
        return count;
    }

    const char* c_str() const
    {
        static const char* nullString = "";
        return buffer ? buffer : nullString;
    }

    void clear()
    {
        count = 0;
    }

    int capacity() const
    {
        return allocated;
    }

    void append(const char* txt, int len)
    {
        if (!len)
            return;
        SWAG_ASSERT(txt);
        reserve(count + len + 1);
        memcpy(buffer + count, txt, len + 1);
        count += len;
    }

    void append(const char* txt)
    {
        if (!txt)
            return;
        int len = (int) strlen(txt);
        if (!len)
            return;
        reserve(count + len + 1);
        memcpy(buffer + count, txt, len + 1);
        count += len;
    }

    void append(const Utf8& txt)
    {
        int len = txt.count;
        if (!len)
            return;
        reserve(count + len + 1);
        memcpy(buffer + count, txt.buffer, len + 1);
        count += len;
    }

    void append(char c)
    {
        reserve(count + 2);
        buffer[count]     = c;
        buffer[count + 1] = 0;
        count++;
    }

    const char* begin() const
    {
        return buffer;
    }

    const char* end() const
    {
        return buffer + count;
    }

    void operator=(const char* txt)
    {
        clear();
        append(txt);
    }

    friend bool operator<(const Utf8& txt1, const Utf8& txt2)
    {
        if (txt1.buffer && !txt2.buffer)
            return false;
        if (!txt1.buffer && txt2.buffer)
            return true;
        if (txt1.buffer == txt2.buffer)
            return false;
        return strcmp(txt1.buffer, txt2.buffer) < 0;
    }

    void operator+=(const Utf8& txt)
    {
        append(txt);
    }

    void operator+=(const char* txt)
    {
        append(txt);
    }

    void operator=(const Utf8& other)
    {
        clear();
        append(other);
    }

    operator const char*()
    {
        return buffer;
    }

    char& operator[](int index)
    {
        SWAG_ASSERT(index < count);
        SWAG_ASSERT(buffer);
        return buffer[index];
    }

    char operator[](int index) const
    {
        return index < count ? buffer[index] : 0;
    }

    friend Utf8 operator+(const Utf8& str1, const char* str2)
    {
        Utf8 result{str1};
        result.append(str2);
        return move(result);
    }

    friend Utf8 operator+(const char* str1, const Utf8& str2)
    {
        Utf8 result{str1};
        result.append(str2);
        return move(result);
    }

    friend bool operator==(const Utf8& str1, const char* str2)
    {
        SWAG_ASSERT(str2);
        if (str1.count == 0)
            return false;
        return !strcmp(str1.buffer, str2);
    }

    friend bool operator!=(const Utf8& str1, const char* str2)
    {
        SWAG_ASSERT(str2);
        if (str1.count == 0)
            return true;
        return strcmp(str1.buffer, str2) != 0;
    }

    friend bool operator==(const Utf8& str1, const Utf8& str2)
    {
        if (str1.count != str2.count)
            return false;
        if (str1.count == 0)
            return true;
        return !strcmp(str1.buffer, str2.buffer);
    }

    friend bool operator!=(const Utf8& str1, const Utf8& str2)
    {
        if (str1.count != str2.count)
            return true;
        if (str1.count == 0)
            return false;
        return strcmp(str1.buffer, str2.buffer) != 0;
    }

    void operator=(Utf8&& from)
    {
        buffer      = from.buffer;
        count       = from.count;
        allocated   = from.allocated;
        from.buffer = nullptr;
        from.count = from.allocated = 0;
    }

    void operator=(char32_t c)
    {
        clear();
        if (!(c & 0xFFFFFF80))
        {
            append((char) c);
            return;
        }

        try
        {
            append(toUtf8.to_bytes(c));
        }
        catch (...)
        {
            append("?");
        }
    }

    void operator+=(char32_t c)
    {
        if (!(c & 0xFFFFFF80))
        {
            append((char) c);
            return;
        }

        try
        {
            append(toUtf8.to_bytes(c));
        }
        catch (...)
        {
            append("?");
        }
    }

    char back() const
    {
        SWAG_ASSERT(buffer && count);
        return buffer[count - 1];
    }

    void makeUpper()
    {
        for (int i = 0; i < count; i++)
        {
            buffer[i] = (char) toupper(buffer[i]);
        }
    }

    void replaceAll(char src, char dst)
    {
        for (int i = 0; i < count; i++)
        {
            if (buffer[i] == src)
                buffer[i] = dst;
        }
    }

    void trimLeft()
    {
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

    void trimRight()
    {
        while (count && UTF8_IS_BLANK(buffer[count]))
            count--;
    }

    void trim()
    {
        trimLeft();
        trimRight();
    }

    void resize(int newSize)
    {
        reserve(newSize + 1);
        count = newSize;
    }

    void pop_back()
    {
        SWAG_ASSERT(count);
        count--;
        if (count)
            buffer[count] = 0;
    }

    int find(const char* str) const
    {
        auto pz = strstr(buffer, str);
        if (!pz)
            return -1;
        return (int) (pz - buffer);
    }
};
