#pragma once
#include "Utf8.h"

struct Global
{
    Global();

    fs::path exePath;
    int      numCores = 0;
};

extern struct Global        g_Global;
extern struct CommandLine   g_CommandLine;
extern struct Log           g_Log;
extern struct Error         g_Error;
extern struct Stats         g_Stats;
extern struct ThreadManager g_ThreadMgr;
extern struct LanguageSpec  g_LangSpec;
extern struct PoolFactory   g_Pool;
extern struct Workspace     g_Workspace;

extern Utf8 format(const char* format, ...);

#define SWAG_CHECK(__expr) \
    {                      \
        if (!(__expr))     \
            return false;  \
    }
#define SWAG_VERIFY(__expr, __err) \
    {                              \
        if (!(__expr))             \
        {                          \
            __err;                 \
            return false;          \
        }                          \
    }