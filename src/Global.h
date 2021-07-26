#pragma once
#include "Utf8.h"
#include "Assert.h"

struct Global
{
    atomic<int> uniqueID = 0;
    bool        exiting  = false;
};

extern struct Global g_Global;