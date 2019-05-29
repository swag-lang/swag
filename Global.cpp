#include "pch.h"
#include "Global.h"
#include "CommandLine.h"
#include "Log.h"
#include "Stats.h"
#include "ThreadManager.h"
#include "TypeManager.h"
#include "LanguageSpec.h"
#include "PoolFactory.h"
#include "PoolFactory.h"
#include "ByteCodeRun.h"
#include "Workspace.h"

Log           g_Log;
Global        g_Global;
CommandLine   g_CommandLine;
Stats         g_Stats;
ThreadManager g_ThreadMgr;
TypeManager   g_TypeMgr;
LanguageSpec  g_LangSpec;
PoolFactory   g_Pool;
Workspace     g_Workspace;
ByteCodeRun   g_Run;

void Global::setup()
{
	g_Run.setup();
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