#pragma once
#include "Pool.h"

struct ConcatBucket : PoolElement
{
    void reset()
    {
    }

    uint8_t*      datas      = nullptr;
    ConcatBucket* nextBucket = nullptr;
    int           count      = 0;
};
