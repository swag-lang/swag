#include "pch.h"
#include "Global.h"
#include "TypeManager.h"
#include "LanguageSpec.h"

Global g_Global;

void Global::setup()
{
    g_TypeMgr.setup();
    g_LangSpec.setup();
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
        if (*pz)
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

void concatForC(Utf8& dst, Utf8& src)
{
    dst.reserve(dst.length() + src.length() + 1);
    const char* pz   = src.c_str();
    char        last = 0;
    while (*pz)
    {
        switch (*pz)
        {
        case '.':
        case '(':
        case ')':
        case '-':
        case ',':
        case ' ':
            if (last != '_')
                dst += '_';
            last = '_';
            break;

        case '*':
        case '>':
            dst += 'P';
            last = 0;
            break;
        case '[':
        case ']':
            dst += 'A';
            last = 0;
            break;

        default:
            dst += *pz;
            last = 0;
            break;
        }

        pz++;
    }
}

void* doByteCodeLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    u |= 1;
    return (void*) u;
}

void* undoByteCodeLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    SWAG_ASSERT(u & 1);
    u ^= 1;
    return (void*) u;
}

bool isByteCodeLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    return u & 1;
}
