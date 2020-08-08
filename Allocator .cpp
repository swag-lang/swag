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
    SWAG_ASSERT((size & 7) == 0);

    int bucket = size / 8;

    // Try in the list of free blocks, per bucket
    if (bucket < MAX_FREE_BUCKETS)
    {
        if (freeBuckets[bucket])
        {
            g_Stats.wastedAllocatorMemory -= size;
            auto result         = freeBuckets[bucket];
            freeBuckets[bucket] = *(void**) result;
            return result;
        }
    }

    // Try the first free block
    if (firstFreeBlock)
    {
        if (size <= firstFreeBlock->size)
        {
            auto remainSize = firstFreeBlock->size - size;
            if (remainSize > 32)
            {
                auto split           = (uint8_t*) firstFreeBlock + size;
                auto result          = firstFreeBlock;
                auto next            = firstFreeBlock->next;
                firstFreeBlock       = (FreeBlock*) split;
                firstFreeBlock->size = remainSize;
                firstFreeBlock->next = next;
                g_Stats.wastedAllocatorMemory -= size;
                return result;
            }
            else if (!lastBucket || lastBucket->maxUsed + size >= lastBucket->allocated)
            {
                g_Stats.wastedAllocatorMemory -= firstFreeBlock->size;
                auto result    = firstFreeBlock;
                firstFreeBlock = firstFreeBlock->next;
                return result;
            }
        }
    }

    // Do we need to allocate a new data block ?
    if (!lastBucket || lastBucket->maxUsed + size >= lastBucket->allocated)
    {
        // We get the remaining space in the last bucket, to be able to use it as
        // a free block
        if (lastBucket)
        {
            auto remain = lastBucket->allocated - lastBucket->maxUsed;
            g_Stats.wastedAllocatorMemory += remain;

            bucket = remain / 8;
            if (bucket < MAX_FREE_BUCKETS)
            {
                auto next                     = freeBuckets[bucket];
                freeBuckets[bucket]           = lastBucket->data + lastBucket->maxUsed;
                *(void**) freeBuckets[bucket] = next;
            }
            else
            {
                auto next            = firstFreeBlock;
                firstFreeBlock       = (FreeBlock*) (lastBucket->data + lastBucket->maxUsed);
                firstFreeBlock->size = remain;
                firstFreeBlock->next = next;
            }
        }

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

    g_Stats.wastedAllocatorMemory += size;

    auto bsize = size / 8;
    if (bsize < MAX_FREE_BUCKETS)
    {
        *(void**) ptr      = freeBuckets[bsize];
        freeBuckets[bsize] = ptr;
        return;
    }

    auto fptr      = (FreeBlock*) ptr;
    fptr->size     = size;
    fptr->next     = firstFreeBlock;
    firstFreeBlock = fptr;
}