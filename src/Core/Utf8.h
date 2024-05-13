// ReSharper disable CppNonExplicitConvertingConstructor
#pragma once
#include "Core/Vector.h"
#include "Core/VectorNative.h"

struct Path;
struct SwagSlice;

#define SWAG_IS_DIGIT(__c)       ((__c) >= '0' && (__c) <= '9')
#define SWAG_IS_ALPHA_HEX(__c)   (((__c) >= 'a' && (__c) <= 'f') || ((__c) >= 'A' && (__c) <= 'F'))
#define SWAG_IS_HEX(__c)         (SWAG_IS_ALPHA_HEX(__c) || SWAG_IS_DIGIT(__c))
#define SWAG_IS_ALPHA(__c)       (((__c) >= 'a' && (__c) <= 'z') || ((__c) >= 'A' && (__c) <= 'Z'))
#define SWAG_IS_NUM_SEP(__c)     ((__c) == '_')
#define SWAG_IS_NOT_NUM_SEP(__c) ((__c) != '_')
#define SWAG_IS_BLANK(__c)       ((__c) == ' ' || (__c) == '\t' || (__c) == '\v' || (__c) == '\f' || (__c) == '\r')
#define SWAG_IS_EOL(__c)         ((__c) == '\n')
#define SWAG_IS_NOT_EOL(__c)     ((__c) != '\n')
#define SWAG_IS_WIN_EOL(__c)     ((__c) == '\r')
#define SWAG_IS_NOT_WIN_EOL(__c) ((__c) != '\r')
#define SWAG_IS_AL_NUM(__c)      (SWAG_IS_ALPHA(__c) || SWAG_IS_DIGIT(__c))

struct Utf8
{
    static constexpr char32_t MAX_ENCODED_UNICODE = 0x10FFFF;

             Utf8() = default;
             Utf8(const char* from);
             Utf8(const Utf8& from);
             Utf8(const std::string& from);
    explicit Utf8(const SwagSlice& slice);
    explicit Utf8(const char* from, uint32_t len);
    explicit Utf8(const Utf8& from, uint32_t capacity);
    explicit Utf8(Utf8&& from) noexcept;
    ~        Utf8();

    void setView(const char* txt, uint32_t len);
    void setView(const Utf8& other);

    void freeBuffer() const;
    void release();
    void reserve(uint32_t newSize);
    void resize(uint32_t newSize);

    const char* c_str() const;
    char*       data() const { return buffer; }

    uint32_t    hash() const;
    bool        empty() const;
    uint32_t    length() const;
    Utf8        toZeroTerminated() const;
    uint32_t    capacity() const;
    const char* begin() const;
    const char* end() const;
    char        back() const;
    bool        containsNoCase(const Utf8& str) const;
    int         find(const Utf8& str, uint32_t startPos = 0) const;
    bool        compareNoCase(const Utf8& txt1) const;
    bool        startsWith(const char* pz) const;
    int         countOf(char c) const;
    int         toInt(uint32_t offset = 0) const;

    void clear();
    void makeLocal();
    void append(const char* txt, uint32_t len);
    void append(const char* txt);
    void append(const Utf8& txt);
    void append(char c);
    void append(uint32_t utf);
    void makeUpper();
    void makeLower();
    void replaceAll(char src, char dst);
    void replaceAll(const Utf8& src, const Utf8& dst);
    void trimLeft();
    void trimRight();
    void trim();
    void removeBack();
    void remove(uint32_t index, uint32_t len);
    void insert(uint32_t index, const char* str);
    void insert(uint32_t index, char c);

    // ReSharper disable once CppNonExplicitConversionOperator
    operator const char*() const;
    char        operator[](uint32_t index) const;
    Utf8&       operator=(const char* txt);
    Utf8&       operator=(const Utf8& other);
    Utf8&       operator=(Utf8&& other) noexcept;
    Utf8&       operator=(uint32_t c);
    friend bool operator<(const Utf8& txt1, const Utf8& txt2);
    friend Utf8 operator+(const Utf8& str1, const char* str2);
    friend bool operator==(const Utf8& str1, char c);
    friend bool operator==(const Utf8& str1, const char* str2);
    friend bool operator==(const Utf8& str1, const Utf8& str2);
    friend bool operator!=(const Utf8& str1, const char* str2);
    friend bool operator!=(const Utf8& str1, const Utf8& str2);
    void        operator+=(const Utf8& txt);
    void        operator+=(const char* txt);
    void        operator+=(uint32_t c);
    void        operator+=(char c);

    void toUni16(VectorNative<uint16_t>& uni, int maxChars = -1) const;
    void toUni32(VectorNative<uint32_t>& uni, int maxChars = -1) const;
    bool toChar32(uint32_t& ch) const;
    void replace(const char* src, const char* dst);

    static const char* decodeUtf8(const char* pz, uint32_t& wc, unsigned& offset);
    static void        tokenize(const Utf8& str, char c, Vector<Utf8>& tokens, bool keepEmpty = false, bool trim = false);
    static void        wordWrap(const Utf8& str, Vector<Utf8>& tokens, int maxLength);
    static void        tokenizeBlanks(const Utf8& str, Vector<Utf8>& tokens);
    static Utf8        toStringF64(double v);
    static Utf8        toNiceSize(size_t size);
    static uint32_t    fuzzyCompare(const Utf8& str1, const Utf8& str2);
    static Utf8        truncateDisplay(const char* str, int maxLen);
    static bool        isNumber(const char* pz);

    char*    buffer    = nullptr;
    uint32_t count     = 0;
    uint32_t allocated = 0;
};

struct HashUtf8
{
    size_t operator()(const Utf8& node) const
    {
        return node.hash();
    }
};

Utf8 form(const char* format, va_list args);
Utf8 form(const char* format, ...);
