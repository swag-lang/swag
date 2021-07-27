#pragma once
#include "Assert.h"
#include "VectorNative.h"

#define SWAG_IS_DIGIT(__c) (__c >= '0' && __c <= '9')
#define SWAG_IS_ALPHAHEX(__c) ((__c >= 'a' && __c <= 'f') || (__c >= 'A' && __c <= 'F'))
#define SWAG_IS_HEX(__c) (SWAG_IS_ALPHAHEX(__c) || SWAG_IS_DIGIT(__c))
#define SWAG_IS_ALPHA(__c) ((__c >= 'a' && __c <= 'z') || (__c >= 'A' && __c <= 'Z'))
#define SWAG_IS_NUMSEP(__c) (__c == '_')
#define SWAG_IS_BLANK(__c) (__c == ' ' || __c == '\t' || __c == '\v' || __c == '\f' || __c == '\r')
#define SWAG_IS_EOL(__c) (__c == '\n')

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

    uint32_t    hash() const;
    void        reset();
    void        reserve(int newSize);
    void        resize(int newSize);
    bool        empty() const;
    int         length() const;
    const char* c_str() const;
    void        clear();
    int         capacity() const;
    void        append(const char* txt, int len);
    void        append(const char* txt);
    void        append(const Utf8& txt);
    void        append(char c);
    void        append(uint32_t utf);
    const char* begin() const;
    const char* end() const;
    char        back() const;
    void        makeUpper();
    void        replaceAll(char src, char dst);
    void        trimLeft();
    void        trimRight();
    void        trim();
    void        pop_back();
    int         find(const char* str, int startpos = 0) const;
    void        remove(int index, int len);
    void        insert(int index, const char* str);
    bool        compareNoCase(const Utf8& txt1);

                operator const char*();
    void        operator=(const char* txt);
    friend bool operator<(const Utf8& txt1, const Utf8& txt2);
    void        operator+=(const Utf8& txt);
    void        operator+=(const char* txt);
    void        operator=(const Utf8& other);
    char        operator[](int index) const;
    friend Utf8 operator+(const Utf8& str1, const char* str2);
    friend Utf8 operator+(const char* str1, const Utf8& str2);
    friend bool operator==(const Utf8& str1, char c);
    friend bool operator==(const Utf8& str1, const char* str2);
    friend bool operator!=(const Utf8& str1, const char* str2);
    friend bool operator==(const Utf8& str1, const Utf8& str2);
    friend bool operator!=(const Utf8& str1, const Utf8& str2);
    void        operator=(Utf8&& from);
    void        operator=(uint32_t c);
    void        operator+=(uint32_t c);
    void        operator+=(char c);

    void toUni16(VectorNative<uint16_t>& uni, int maxChars = -1);
    void toUni32(VectorNative<uint32_t>& uni, int maxChars = -1);
    bool toChar32(uint32_t& ch);
    void replace(const char* src, const char* dst);

    static uint32_t    hash(const char* buffer, int count);
    static const char* decodeUtf8(const char* pz, uint32_t& c, unsigned& offset);
    static Utf8        format(const char* format, ...);
    static string      normalizePath(const fs::path& path);
    static void        tokenize(const char* str, char c, vector<Utf8>& tokens);
    static void        tokenizeBlanks(const char* str, vector<Utf8>& tokens);
    static Utf8        toStringF64(double v);
    static Utf8        toNiceSize(size_t size);
    static uint32_t    fuzzyCompare(const Utf8& str1, const Utf8& str2);
};
