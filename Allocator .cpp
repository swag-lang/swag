#include "pch.h"
#include "Allocator.h"
#include "CommandLine.h"

thread_local Allocator g_Allocator;

void* operator new(size_t t)
{
    t           = g_Allocator.alignSize((int) t);
    uint64_t* p = (uint64_t*) g_Allocator.alloc((uint32_t) t + sizeof(uint64_t));
    *p          = (uint64_t) t;
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

void* Allocator::tryFreeBlock(uint32_t maxCount, int size)
{
    if (!firstFreeBlock)
        return nullptr;

    FreeBlock* prevBlock = nullptr;
    auto       tryBlock  = firstFreeBlock;
    uint32_t   cpt       = 0;

    while (tryBlock && cpt++ < maxCount)
    {
        if (size <= tryBlock->size)
        {
            auto remainSize = tryBlock->size - size;
            if (remainSize > 32)
            {
                g_Stats.wastedMemory -= size;
                auto bucket = remainSize / 8;
                auto split  = (uint8_t*) tryBlock + size;
                auto result = tryBlock;

                if (bucket < MAX_FREE_BUCKETS)
                {
                    auto next                     = freeBuckets[bucket];
                    freeBuckets[bucket]           = split;
                    *(void**) freeBuckets[bucket] = next;
                    freeBucketsCpt[bucket]++;

                    if (prevBlock)
                        prevBlock->next = tryBlock->next;
                    else
                        firstFreeBlock = tryBlock->next;
                }
                else
                {
                    auto next       = tryBlock->next;
                    auto splitBlock = (FreeBlock*) split;
                    if (prevBlock)
                        prevBlock->next = splitBlock;
                    else
                        firstFreeBlock = splitBlock;
                    splitBlock->size = remainSize;
                    splitBlock->next = next;
                }

                return result;
            }
            else
            {
                g_Stats.wastedMemory -= tryBlock->size;

                auto result = tryBlock;
                if (prevBlock)
                    prevBlock->next = tryBlock->next;
                else
                    firstFreeBlock = tryBlock->next;

                return result;
            }
        }

        prevBlock = tryBlock;
        tryBlock  = tryBlock->next;
        SWAG_ASSERT(tryBlock != firstFreeBlock);
    }

    return nullptr;
}

void* Allocator::tryBucket(uint32_t bucket, int size)
{
    if (bucket >= MAX_FREE_BUCKETS)
        return nullptr;
    if (!freeBuckets[bucket])
        return nullptr;

    // If the bucket size is greater than the real size, then put the remaining
    // memory size in the corresponding bucket
    auto wasted = (bucket * 8) - size;
    if (wasted > 32)
    {
        auto wastedBucket         = wasted / 8;
        auto ptr                  = (int8_t*) freeBuckets[bucket] + size;
        *(void**) ptr             = freeBuckets[wastedBucket];
        freeBuckets[wastedBucket] = ptr;
        freeBucketsCpt[wastedBucket]--;
    }

    g_Stats.wastedMemory -= bucket * 8;
    auto result         = freeBuckets[bucket];
    freeBuckets[bucket] = *(void**) result;
    if (g_CommandLine.devMode)
        memset(result, 0xCC, size);
    freeBucketsCpt[bucket]--;

    return result;
}

void* Allocator::alloc(int size)
{
    SWAG_ASSERT((size & 7) == 0);

    int bucket = size / 8;
    if (bucket < MAX_FREE_BUCKETS)
    {
        // Try in the list of free blocks, per bucket
        auto result = tryBucket(bucket, size);
        if (result)
            return result;
    }

    // Try the first free block
    auto result = tryFreeBlock(1, size);
    if (result)
    {
        if (g_CommandLine.devMode)
            memset(result, 0xCC, size);
        return result;
    }

    // Do we need to allocate a new data block ?
    if (!lastBucket || lastBucket->maxUsed + size >= lastBucket->allocated)
    {
        // Magic number
        result = tryFreeBlock(8, size);
        if (result)
        {
            if (g_CommandLine.devMode)
                memset(result, 0xCC, size);
            return result;
        }

        // We get the remaining space in the last bucket, to be able to use it as
        // a free block
        if (lastBucket)
        {
            auto remain = lastBucket->allocated - lastBucket->maxUsed;
            g_Stats.wastedMemory += remain;

            bucket = remain / 8;
            if (bucket < MAX_FREE_BUCKETS)
            {
                auto next           = freeBuckets[bucket];
                freeBuckets[bucket] = lastBucket->data + lastBucket->maxUsed;
                freeBucketsCpt[bucket]++;
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

    if (g_CommandLine.devMode)
        memset(ptr, 0xFE, size);

    g_Stats.wastedMemory += size;

    auto bsize = size / 8;
    if (bsize < MAX_FREE_BUCKETS)
    {
        *(void**) ptr      = freeBuckets[bsize];
        freeBuckets[bsize] = ptr;
        freeBucketsCpt[bsize]++;
        return;
    }

    auto fptr      = (FreeBlock*) ptr;
    fptr->size     = size;
    fptr->next     = firstFreeBlock;
    firstFreeBlock = fptr;
}