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
    vector<char> vec(len + 1);
    va_start(args, format);
    vsnprintf(&vec[0], len + 1, format, args);
    va_end(args);
    return &vec[0];
}

void makeUpper(string& str)
{
    auto len = str.length();
    for (int i = 0; i < len; i++)
    {
        auto& c = str[i];
        c       = (char) toupper(c);
    }
}

void replaceAll(string& str, char src, char dst)
{
    auto len = str.length();
    for (int i = 0; i < len; i++)
    {
        auto& c = str[i];
        if (c == src)
            c = dst;
    }
}

void tokenize(const char* str, char c, vector<string>& tokens)
{
    auto pz = str;

    tokens.clear();
    while (*pz)
    {
        string one;
        while (*pz && *pz != c)
            one += *pz++;
        if (*pz)
            pz++;
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