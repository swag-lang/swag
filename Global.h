#pragma once
#include "Utf8.h"
#include "Assert.h"

struct Global
{
    void        setup();
    atomic<int> uniqueID;
};

extern Utf8    format(const char* format, ...);
extern void    makeUpper(string& str);
extern void    replaceAll(string& str, char src, char dst);
extern string  normalizePath(const fs::path& path);
extern void    tokenize(const char* str, char c, vector<string>& tokens);
extern void    tokenizeBlanks(const char* str, vector<string>& tokens);
extern wstring utf8ToUnicode(const string& s);
extern void concatForC(Utf8& dst, Utf8& src);

extern string toStringF64(double v);

extern struct Global g_Global;

#define SWAG_CHECK(__expr) \
    do                     \
    {                      \
        if (!(__expr))     \
            return false;  \
    } while (0)
#define SWAG_VERIFY(__expr, __err) \
    {                              \
        if (!(__expr))             \
        {                          \
            __err;                 \
            return false;          \
        }                          \
    }
