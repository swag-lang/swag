#pragma once
#include "Pool.h"

static const int CONCAT_BUCKET_SIZE = 64 * 1024;
struct ConcatBucket : PoolElement
{
    uint8_t*      datas;
    int           count      = 0;
    ConcatBucket* nextBucket = nullptr;
};
