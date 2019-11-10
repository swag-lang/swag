#pragma once
#include "Pool.h"

struct ConcatBucket : PoolElement
{
    uint8_t*      datas;
    ConcatBucket* nextBucket = nullptr;
    int           count      = 0;
};
