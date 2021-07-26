#pragma once
#include "Assert.h"
#include "VectorNative.h"
#define UTF8_IS_BLANK(__c) (__c == ' ' || __c == '\t' || __c == '\v' || __c == '\f' || __c == '\r')

struct Utf8
{
    char* buffer    = nullptr;
    int   count     = 0;
    int   allocated = 0;

    Utf8();
    Utf8(const char* from);
    Utf8(const char* from, uint32_t len);
    Utf8(const string& from);
    Utf8(const Utf8& from);
    Utf8(Utf8&& from);
    ~Utf8();

    uint32_t        hash() const;
    static uint32_t hash(const char* buffer, int count);
    void            reset();
    void            reserve(int newSize);
    bool            empty() const;
    int             length() const;
    const char*     c_str() const;
    void            clear();
    int             capacity() const;
    void            append(const char* txt, int len);
    void            append(const char* txt);
    void            append(const Utf8& txt);
    void            append(char c);
    void            append(uint32_t utf);
    void            appendTmpChar(char c);
    void            setTrailingZero();
    const char*     begin() const;
    const char*     end() const;
                    operator const char*();
    char            back() const;
    void            makeUpper();
    void            replaceAll(char src, char dst);
    void            trimLeft();
    void            trimRight();
    void            trim();
    void            resize(int newSize);
    void            pop_back();
    int             find(const char* str, int startpos = 0) const;
    void            remove(int index, int len);
    void            insert(int index, const char* str);

    void        operator=(const char* txt);
    friend bool operator<(const Utf8& txt1, const Utf8& txt2);
    void        operator+=(const Utf8& txt);
    void        operator+=(const char* txt);
    void        operator=(const Utf8& other);
    char        operator[](int index) const;
    friend Utf8 operator+(const Utf8& str1, const char* str2);
    friend Utf8 operator+(const char* str1, const Utf8& str2);
    friend bool operator==(const Utf8& str1, const char* str2);
    friend bool operator!=(const Utf8& str1, const char* str2);
    friend bool operator==(const Utf8& str1, const Utf8& str2);
    friend bool operator!=(const Utf8& str1, const Utf8& str2);
    void        operator=(Utf8&& from);
    void        operator=(uint32_t c);
    void        operator+=(uint32_t c);
    void        operator+=(char c);

    void               toUni16(VectorNative<uint16_t>& uni, int maxChars = -1);
    void               toUni32(VectorNative<uint32_t>& uni, int maxChars = -1);
    static const char* transformUtf8ToChar32(const char* pz, uint32_t& c);
    bool               toChar32(uint32_t& ch);
    void               replace(const char* src, const char* dst);
};
