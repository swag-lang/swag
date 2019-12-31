#pragma once
#include "Assert.h"
#include "Stats.h"
#define ALLOCATOR_BUCKET_SIZE 1024 * 1024
#define MAX_FREE_BUCKETS 512 / 8

struct AllocatorBucket
{
    uint8_t* data      = nullptr;
    int      maxUsed   = 0;
    int      allocated = 0;
};

struct Allocator
{
    Allocator()
    {
        memset(freeBuckets, 0, sizeof(freeBuckets));
    }

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

        // Must be done before ::new because ::new can allocate too !
        currentData += sizeof(T);
        lastBucket->maxUsed += sizeof(T);

        ::new (returnData) T;
        return (T*) returnData;
    }

    int   alignSize(int size);
    void  free(void*, int size);
    void* alloc(int size);

    AllocatorBucket* lastBucket  = nullptr;
    uint8_t*         currentData = nullptr;
    void*            freeBuckets[MAX_FREE_BUCKETS];
};

extern thread_local Allocator g_Allocator;