#include "pch.h"
#include "Module.h"
#include "Concat.h"

Concat::Concat()
{
    checkCount(1);
}

void Concat::checkCount(int offset)
{
    if (lastBucket && lastBucket->count + offset < CONCAT_BUCKET_SIZE)
        return;

    lastBucket = poolFactory.concatBucket.alloc();
    if (!firstBucket)
        firstBucket = lastBucket;
    currentSP = lastBucket->datas;
}

void Concat::addU8(uint8_t v)
{
    checkCount(1);
    *(uint8_t*) currentSP = v;
    currentSP += 1;
}