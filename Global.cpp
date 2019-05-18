#include "pch.h"
#include "Global.h"
#include "CommandLine.h"
#include "Log.h"
#include "Stats.h"
#include "ThreadManager.h"
#include "LanguageSpec.h"
#include "PoolFactory.h"

Log           g_Log;
Global        g_Global;
CommandLine   g_CommandLine;
Stats         g_Stats;
ThreadManager g_ThreadMgr;
LanguageSpec  g_LangSpec;
PoolFactory   g_Pool;

Global::Global()
{
    numCores = std::thread::hardware_concurrency();
}

wstring format(const wchar_t* format, ...)
{
    va_list args;
    va_start(args, format);
    size_t len = vswprintf(nullptr, 0, format, args);
    va_end(args);
    vector<wchar_t> vec(len + 1);
    va_start(args, format);
    vswprintf(&vec[0], len + 1, format, args);
    va_end(args);
    return &vec[0];
}