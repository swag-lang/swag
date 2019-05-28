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

    uint8_t* currentSP = nullptr;
};