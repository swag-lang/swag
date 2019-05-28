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
	{
		lastBucket->count += offset;
		return;
	}

    lastBucket = poolFactory.concatBucket.alloc();
    if (!firstBucket)
        firstBucket = lastBucket;
    currentSP = lastBucket->datas;
	lastBucket->count = offset;
}

void Concat::addU8(uint8_t v)
{
    checkCount(1);
    *(uint8_t*) currentSP = v;
    currentSP += sizeof(uint8_t);
}

void Concat::addU16(uint16_t v)
{
    checkCount(2);
    *(uint16_t*) currentSP = v;
    currentSP += sizeof(uint16_t);
}

void Concat::addS32(int32_t v)
{
    checkCount(4);
    *(int32_t*) currentSP = v;
    currentSP += sizeof(int32_t);
}