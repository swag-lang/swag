#pragma once
#include "Utf8.h"
#include "Assert.h"

struct Global
{
    void        setup();
    atomic<int> uniqueID        = 0;
    atomic<int> compilerAllocTh = 0;
    bool        exiting         = false;
};

extern struct Global g_Global;