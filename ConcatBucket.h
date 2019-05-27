#pragma once
#include "Pool.h"

static const int CONCAT_BUCKET_SIZE = 1024;
struct ConcatBucket : PoolElement
{
    uint8_t       datas[CONCAT_BUCKET_SIZE];
    int           count      = 0;
    ConcatBucket* nextBucket = nullptr;
};
