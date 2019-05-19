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

utf8 format(const char* format, ...)
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