#include "pch.h"
#include "Allocator.h"

thread_local Allocator g_Allocator;

void* operator new(size_t t)
{
    t           = g_Allocator.alignSize((int) t);
    uint64_t* p = (uint64_t*) g_Allocator.alloc((uint32_t) t + sizeof(uint64_t));
    *p          = (uint64_t) t;
    g_Stats.wastedAllocatorMemory += sizeof(uint64_t);
    return p + 1;
}

void operator delete(void* addr)
{
    if (!addr)
        return;
    uint64_t* p = (uint64_t*) addr;
    p--;
    return g_Allocator.free(p, (int) *p);
}

void* Allocator::alloc(int size)
{
    // Must be aligned
    if ((size & 7) == 0)
    {
        int bucket = size / 8;
        if (bucket < MAX_FREE_BUCKETS)
        {
            if (freeBuckets[bucket])
            {
                auto result         = freeBuckets[bucket];
                freeBuckets[bucket] = *(void**) result;
                return result;
            }
        }
    }

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

int Allocator::alignSize(int size)
{
    if (!size)
        return 0;
    auto alignedSize = size & ~7;
    if (size != alignedSize)
        return alignedSize + 8;
    return size;
}

void Allocator::free(void* ptr, int size)
{
    if (!ptr)
        return;
    SWAG_ASSERT(!(size & 7));

    auto bsize = size / 8;
    if (bsize >= MAX_FREE_BUCKETS)
        return;
    *(void**) ptr      = freeBuckets[bsize];
    freeBuckets[bsize] = ptr;
}