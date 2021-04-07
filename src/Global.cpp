#include "pch.h"
#include "Global.h"
#include "TypeManager.h"
#include "LanguageSpec.h"
#include "Backend.h"

Global g_Global;

void Global::setup()
{
    g_TypeMgr.setup();
    g_LangSpec.setup();
    Backend::setup();
}

Utf8 format(const char* format, ...)
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
    return move(vec);
}

void tokenize(const char* str, char c, vector<Utf8>& tokens)
{
    auto pz = str;

    tokens.clear();
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

void tokenizeBlanks(const char* str, vector<Utf8>& tokens)
{
    auto pz = str;

    tokens.clear();
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

string normalizePath(const fs::path& path)
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

Utf8 toStringF64(double v)
{
    Utf8 s = format("%.35lf", v);
    while (s.buffer[s.count - 1] == '0')
    {
        s.buffer[s.count - 1] = 0;
        s.count--;
    }

    s += "0";
    return s;
}

void* doForeignLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    u |= SWAG_LAMBDA_FOREIGN_MARKER;
    return (void*) u;
}

void* undoForeignLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    SWAG_ASSERT(u & SWAG_LAMBDA_FOREIGN_MARKER);
    u ^= SWAG_LAMBDA_FOREIGN_MARKER;
    return (void*) u;
}

bool isForeignLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    return u & SWAG_LAMBDA_FOREIGN_MARKER;
}

void* doByteCodeLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    u |= SWAG_LAMBDA_BC_MARKER;
    return (void*) u;
}

void* undoByteCodeLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    SWAG_ASSERT(u & SWAG_LAMBDA_BC_MARKER);
    u ^= SWAG_LAMBDA_BC_MARKER;
    return (void*) u;
}

bool isByteCodeLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    return u & SWAG_LAMBDA_BC_MARKER;
}

Utf8 toNiceSize(size_t size)
{
    if (size < 1024)
        return format("%u bytes", size);
    if (size < 1024 * 1024)
        return format("%.1f Kb", size / 1024.0f);
    if (size < 1024 * 1024 * 1024)
        return format("%.1f Mb", size / (1024.0f * 1024.0f));
    return format("%.1f Gb", size / (1024.0f * 1024.0f * 1024.0f));
}

uint32_t fuzzyCompare(const Utf8& str1, const Utf8& str2)
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
