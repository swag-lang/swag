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