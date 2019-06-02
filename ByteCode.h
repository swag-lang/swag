#pragma once
#include "Pool.h"
#include "Concat.h"
#include "Utf8.h"

struct ByteCode : public PoolElement
{
    Concat       out;
    bool         hasDebugInfos = true;
    vector<Utf8> strBuffer;
};
