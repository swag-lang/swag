#include "pch.h"
#include "Global.h"
#include "CommandLine.h"
#include "Log.h"
#include "Stats.h"

Log g_Log;
Global g_Global;
CommandLine g_CommandLine;
Stats g_Stats;

Global::Global()
{
	numCores = std::thread::hardware_concurrency();
}