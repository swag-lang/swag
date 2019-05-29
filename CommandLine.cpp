#include "pch.h"
#include "CommandLine.h"
#include "Global.h"

void CommandLine::setup()
{
    numCores = std::thread::hardware_concurrency();
}
