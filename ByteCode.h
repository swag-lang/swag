#pragma once
#include "Pool.h"
#include "Concat.h"

struct ByteCode : public PoolElement
{
	Concat out;
    bool   hasDebugInfos = true;
};

