#include "pch.h"
#include "Global.h"
#include "CommandLine.h"
#include "Log.h"

Log g_Log;
Global g_Global;
CommandLine g_CommandLine;

Global::Global()
{
	numCores = std::thread::hardware_concurrency();
}