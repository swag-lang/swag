#pragma once
#include "Pool.h"

struct ConcatBucket
{
    uint8_t*      datas      = nullptr;
    ConcatBucket* nextBucket = nullptr;
    int           count      = 0;
};
