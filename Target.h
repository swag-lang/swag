#pragma once
#include "CommandLine.h"

struct Target
{
    string name;
    bool   debugInfos    = false;
    int    optimizeLevel = 0;
};
