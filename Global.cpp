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
    auto str = path.string();
    auto len = str.length();
    for (int i = 0; i < len; i++)
    {
        char c = str[i];
        if (c == '\\')
            str[i] = '/';
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