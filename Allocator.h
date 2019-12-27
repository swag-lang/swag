#pragma once
#include "Assert.h"
#include "Stats.h"
#define ALLOCATOR_BUCKET_SIZE 64 * 1024

struct AllocatorBucket
{
    uint8_t* data      = nullptr;
    int      maxUsed   = 0;
    int      allocated = 0;
};

struct Allocator
{
    template<typename T>
    T* alloc()
    {
        static_assert(sizeof(T) < ALLOCATOR_BUCKET_SIZE);
        if (!lastBucket || lastBucket->maxUsed + sizeof(T) >= ALLOCATOR_BUCKET_SIZE)
        {
            if (lastBucket)
                g_Stats.wastedAllocatorMemory += lastBucket->allocated - lastBucket->maxUsed;
            lastBucket            = (AllocatorBucket*) malloc(sizeof(AllocatorBucket));
            lastBucket->maxUsed   = 0;
            lastBucket->allocated = ALLOCATOR_BUCKET_SIZE;
            lastBucket->data      = (uint8_t*) malloc(ALLOCATOR_BUCKET_SIZE);
            currentData           = lastBucket->data;
            g_Stats.allocatorMemory += lastBucket->allocated;
        }

        auto returnData = currentData;
        ::new (returnData) T;
        currentData += sizeof(T);
        lastBucket->maxUsed += sizeof(T);
        return (T*) returnData;
    }

    void* alloc(int size)
    {
        if (!lastBucket || lastBucket->maxUsed + size >= lastBucket->allocated)
        {
            if (lastBucket)
                g_Stats.wastedAllocatorMemory += lastBucket->allocated - lastBucket->maxUsed;
            lastBucket            = (AllocatorBucket*) malloc(sizeof(AllocatorBucket));
            lastBucket->maxUsed   = 0;
            lastBucket->allocated = max(size, ALLOCATOR_BUCKET_SIZE);
            lastBucket->data      = (uint8_t*) malloc(lastBucket->allocated);
            currentData           = lastBucket->data;
            g_Stats.allocatorMemory += lastBucket->allocated;
        }

        auto returnData = currentData;
        currentData += size;
        lastBucket->maxUsed += size;
        return returnData;
    }

    AllocatorBucket* lastBucket  = nullptr;
    uint8_t*         currentData = nullptr;
};

extern thread_local Allocator g_Allocator;