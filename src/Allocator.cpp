#include "pch.h"
#include "Allocator.h"
#include "CommandLine.h"
#include "Global.h"

const uint64_t MAGIC_ALLOC = 0xC0DEC0DEC0DEC0DE;
const uint64_t MAGIC_FREE  = 0xCAFECAFECAFECAFE;

thread_local Allocator g_Allocator;

atomic<int> g_CompilerAllocTh = 0;
mutex       g_allocatorMutex;
Allocator*  g_sharedAllocator = nullptr;

void* operator new(size_t t)
{
    t           = Allocator::alignSize((int) t + sizeof(uint64_t));
    uint64_t* p = (uint64_t*) g_Allocator.alloc(t);
    *p          = (uint64_t) t;
    return p + 1;
}

void operator delete(void* addr)
{
    if (!addr)
        return;
    uint64_t* p = (uint64_t*) addr;
    p--;
    return g_Allocator.free(p, *p);
}

AllocatorImpl::AllocatorImpl()
{
    memset(freeBuckets, 0, sizeof(freeBuckets));
    memset(freeBucketsCpt, 0, sizeof(freeBucketsCpt));
}

void* AllocatorImpl::tryFreeBlock(uint32_t maxCount, size_t size)
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

void* AllocatorImpl::tryBucket(uint32_t bucket, size_t size)
{
    if (bucket >= MAX_FREE_BUCKETS)
        return nullptr;
    if (!freeBuckets[bucket])
        return nullptr;

    // If the bucket size is greater than the real size, then put the remaining
    // memory size in the corresponding bucket
    auto wasted = (bucket * 8) - size;
    if (wasted)
    {
        auto wastedBucket         = wasted / 8;
        auto ptr                  = (int8_t*) freeBuckets[bucket] + size;
        *(void**) ptr             = freeBuckets[wastedBucket];
        freeBuckets[wastedBucket] = ptr;
        freeBucketsCpt[wastedBucket]++;
    }

    g_Stats.wastedMemory -= bucket * 8;
    auto result         = freeBuckets[bucket];
    freeBuckets[bucket] = *(void**) result;
#ifdef SWAG_DEV_MODE
    memset(result, 0xCC, size);
#endif
    freeBucketsCpt[bucket]--;

    return result;
}

void* AllocatorImpl::alloc(size_t size)
{
    SWAG_ASSERT((size & 7) == 0);

    auto  bucket = size / 8;
    void* result;
    if (bucket < MAX_FREE_BUCKETS)
    {
        // Try in the list of free blocks, per bucket
        result = tryBucket((uint32_t) bucket, size);
        if (result)
            return result;

        // Try to split the biggest bucket
        result = tryBucket(MAX_FREE_BUCKETS - 1, size);
        if (result)
            return result;
    }

    // Try other big buckets
    for (int i = (int) bucket + 1; i <= MAX_FREE_BUCKETS - 2; i++)
    {
        result = tryBucket(i, size);
        if (result)
            return result;
    }

    // Magic number
    uint32_t maxTries = !lastBucket || lastBucket->maxUsed + size >= lastBucket->allocated ? 1024 : 128;
    result            = tryFreeBlock(maxTries, size);
    if (result)
    {
#ifdef SWAG_DEV_MODE
        memset(result, 0xCC, size);
#endif
        return result;
    }

    // Do we need to allocate a new data block ?
    if (!lastBucket || lastBucket->maxUsed + size >= lastBucket->allocated)
    {
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

        g_Stats.allocatorMemory += sizeof(AllocatorBucket);
        g_Stats.allocatorMemory += lastBucket->allocated;
        g_Stats.wastedMemory += lastBucket->allocated;
    }

    auto returnData = currentData;
    currentData += size;
    lastBucket->maxUsed += size;
    g_Stats.wastedMemory -= size;
    return returnData;
}

void AllocatorImpl::free(void* ptr, size_t size)
{
    if (!ptr)
        return;
    SWAG_ASSERT(!(size & 7));

#ifdef SWAG_DEV_MODE
    memset(ptr, 0xFE, size);
#endif

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

Allocator::Allocator()
{
    impl = &_impl;

    // Allocator created by the tls of a user bytecode thread. In that can, we use
    // the same shared AllocatorImpl
    if (g_CompilerAllocTh >= g_Stats.numWorkers && g_Stats.numWorkers)
    {
        shared = true;
        unique_lock lk(g_allocatorMutex);
        if (g_sharedAllocator)
        {
            impl = g_sharedAllocator->impl;
            SWAG_ASSERT(impl);
        }
        else
        {
            g_sharedAllocator       = (Allocator*) malloc(sizeof(Allocator));
            g_sharedAllocator->impl = &g_sharedAllocator->_impl;
            memset(g_sharedAllocator->impl, 0, sizeof(AllocatorImpl));
            impl = &g_sharedAllocator->_impl;
        }
    }
}

size_t Allocator::alignSize(size_t size)
{
    if (!size)
        return 0;
    auto alignedSize = size & ~7;
    if (size != alignedSize)
        return alignedSize + 8;
    return size;
}

void* Allocator::alloc(size_t size)
{
    if (shared)
        g_allocatorMutex.lock();

#ifdef SWAG_CHECK_MEMORY
    auto userSize = size;
    size += 2 * sizeof(uint64_t);
#endif

    uint8_t* result = (uint8_t*) impl->alloc(size);

#ifdef SWAG_CHECK_MEMORY
    *(uint64_t*) result = MAGIC_ALLOC;
    result += sizeof(uint64_t);
    auto end         = result + userSize;
    *(uint64_t*) end = MAGIC_ALLOC;
#endif

    if (shared)
        g_allocatorMutex.unlock();
    return result;
}

void Allocator::free(void* ptr, size_t size)
{
    if (!ptr)
        return;

    if (shared)
        g_allocatorMutex.lock();

    uint8_t* addr = (uint8_t*) ptr;
#ifdef SWAG_CHECK_MEMORY
    auto end = addr + size;
    SWAG_ASSERT(*(uint64_t*) end == MAGIC_ALLOC);
    addr -= sizeof(uint64_t);
    SWAG_ASSERT(*(uint64_t*) addr == MAGIC_ALLOC);
    size += 2 * sizeof(uint64_t);
#endif

    impl->free(addr, size);

    if (shared)
        g_allocatorMutex.unlock();
}