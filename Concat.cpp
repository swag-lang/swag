#include "pch.h"
#include "Concat.h"
#include "PoolFactory.h"

Concat::Concat()
{
	poolFactory = new PoolFactory();
    checkCount(1);
}

void Concat::checkCount(int offset)
{
    if (lastBucket && lastBucket->count + offset < CONCAT_BUCKET_SIZE)
    {
        lastBucket->count += offset;
        return;
    }

    lastBucket = poolFactory->concatBucket.alloc();
    if (!firstBucket)
        firstBucket = lastBucket;
    currentSP         = lastBucket->datas;
    lastBucket->count = offset;
}

void Concat::addBool(bool v)
{
    checkCount(sizeof(bool));
    *(bool*) currentSP = v;
    currentSP += sizeof(bool);
}

void Concat::addU8(uint8_t v)
{
    checkCount(sizeof(uint8_t));
    *(uint8_t*) currentSP = v;
    currentSP += sizeof(uint8_t);
}

void Concat::addU16(uint16_t v)
{
    checkCount(sizeof(uint16_t));
    *(uint16_t*) currentSP = v;
    currentSP += sizeof(uint16_t);
}

void Concat::addU32(uint32_t v)
{
    checkCount(sizeof(uint32_t));
    *(uint32_t*) currentSP = v;
    currentSP += sizeof(uint32_t);
}

void Concat::addU64(uint64_t v)
{
    checkCount(sizeof(uint64_t));
    *(uint64_t*) currentSP = v;
    currentSP += sizeof(uint64_t);
}

void Concat::addS8(int8_t v)
{
    checkCount(sizeof(int8_t));
    *(int8_t*) currentSP = v;
    currentSP += sizeof(int8_t);
}

void Concat::addS16(int16_t v)
{
    checkCount(sizeof(int16_t));
    *(int16_t*) currentSP = v;
    currentSP += sizeof(int16_t);
}

void Concat::addS32(int32_t v)
{
    checkCount(sizeof(int32_t));
    *(int32_t*) currentSP = v;
    currentSP += sizeof(int32_t);
}

void Concat::addS64(int64_t v)
{
    checkCount(sizeof(int64_t));
    *(int64_t*) currentSP = v;
    currentSP += sizeof(int64_t);
}

void Concat::addF32(float v)
{
    checkCount(sizeof(float));
    *(float*) currentSP = v;
    currentSP += sizeof(float);
}

void Concat::addF64(double v)
{
    checkCount(sizeof(double));
    *(double*) currentSP = v;
    currentSP += sizeof(double);
}