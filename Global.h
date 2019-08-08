#pragma once
#include "Utf8.h"

struct Global
{
    void        setup();
    atomic<int> uniqueID;
};

extern Utf8          format(const char* format, ...);
extern void          makeUpper(string& str);
extern void          replaceAll(string& str, char src, char dst);
extern string        normalizePath(const fs::path& path);
extern void          tokenize(const char* str, char c, vector<string>& tokens);
extern wstring       utf8ToUnicode(const string& s);
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

#define SWAG_ASSERT(__expr)                                                                  \
    {                                                                                        \
        if (!(__expr))                                                                       \
        {                                                                                    \
            g_Log.error(format("assertion failed: %s:%d: %s", __FILE__, __LINE__, #__expr)); \
            assert(false);                                                                   \
            exit(-1);                                                                        \
        }                                                                                    \
    }