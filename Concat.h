#pragma once
#include "Pool.h"
#include "PoolFactory.h"
struct ConcatBucket;

struct Concat
{
    Concat();
    void checkCount(int offset);

    void addU8(uint8_t v);
    void addU16(uint16_t v);
    void addS32(int32_t v);

    ConcatBucket* firstBucket = nullptr;
    ConcatBucket* lastBucket  = nullptr;
    PoolFactory   poolFactory;

    uint8_t*      currentSP  = nullptr;
    ConcatBucket* currentSPB = nullptr;
    int           currentSPI;

    inline void rewind()
    {
        currentSPB = firstBucket;
        currentSP  = currentSPB->datas;
        currentSPI = 0;
    }

    inline uint8_t* seek(int offset)
    {
        if (currentSPI + offset > currentSPB->count)
        {
            currentSPI = currentSPI + offset - currentSPB->count;
            currentSPB = currentSPB->nextBucket;
            currentSP  = currentSPB->datas + currentSPI;
        }
        else
        {
            currentSPI += offset;
            currentSP += offset;
        }

		return currentSP;
    }
};