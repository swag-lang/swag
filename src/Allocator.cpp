#include "pch.h"
#include "Allocator.h"
#include "CommandLine.h"
#include "Os.h"
#include "Log.h"
#include "ErrorIds.h"
#include "ScopedLock.h"

const uint64_t ALLOCATOR_BUCKET_SIZE = 1024 * 1024;

#ifdef SWAG_CHECK_MEMORY
const uint64_t MAGIC_ALLOC = 0xC0DEC0DEC0DEC0DE;
const uint64_t MAGIC_FREE  = 0xCAFECAFECAFECAFE;
#endif

thread_local Allocator g_Allocator;
atomic<int>            g_CompilerAllocTh = 0;
mutex                  g_AllocatorMutex;
Allocator*             g_SharedAllocator = nullptr;

void* operator new(size_t t)
{
    t      = Allocator::alignSize((int) t + sizeof(uint64_t));
    auto p = (uint64_t*) g_Allocator.alloc(t);
    *p     = (uint64_t) t;
    return p + 1;
}

void operator delete(void* addr) noexcept
{
    if (!addr)
        return;
    auto p = (uint64_t*) addr;
    p--;
    return g_Allocator.free(p, *p);
}

AllocatorImpl::AllocatorImpl()
{
    memset(freeBuckets, 0, sizeof(freeBuckets));
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
            if (remainSize)
            {
                if (g_CommandLine && g_CommandLine->stats)
                    g_Stats.wastedMemory -= size;
                auto bucket = remainSize / 8;
                auto split  = (uint8_t*) tryBlock + size;
                auto result = tryBlock;

                if (bucket < MAX_FREE_BUCKETS)
                {
                    auto next                     = freeBuckets[bucket];
                    freeBuckets[bucket]           = split;
                    *(void**) freeBuckets[bucket] = next;
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
                if (g_CommandLine && g_CommandLine->stats)
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
    SWAG_ASSERT(bucket < MAX_FREE_BUCKETS);
    SWAG_ASSERT(freeBuckets[bucket]);

    // If the bucket size is greater than the real size, then put the remaining
    // memory size in the corresponding bucket
    auto wasted = (bucket * 8) - size;
    if (wasted)
    {
        auto wastedBucket         = wasted / 8;
        auto ptr                  = (int8_t*) freeBuckets[bucket] + size;
        *(void**) ptr             = freeBuckets[wastedBucket];
        freeBuckets[wastedBucket] = ptr;
        if (g_CommandLine && g_CommandLine->stats)
            g_Stats.wastedMemory += wasted;
    }

    if (g_CommandLine && g_CommandLine->stats)
        g_Stats.wastedMemory -= bucket * 8;
    auto result         = freeBuckets[bucket];
    freeBuckets[bucket] = *(void**) result;
#ifdef SWAG_DEV_MODE
    memset(result, 0xAA, size);
#endif
    return result;
}

void* AllocatorImpl::alloc(size_t size)
{
    SWAG_ASSERT((size & 7) == 0);

    auto  bucket = size / 8;
    void* result = nullptr;
    if (bucket < MAX_FREE_BUCKETS)
    {
        // Try in the list of free blocks, per bucket
        if (freeBuckets[bucket])
        {
            result = tryBucket((uint32_t) bucket, size);
            if (result)
                return result;
        }

        // Try other big buckets
        for (int i = MAX_FREE_BUCKETS - 1; i > (int) bucket; i--)
        {
            if (freeBuckets[i])
            {
                result = tryBucket(i, size);
                if (result)
                    return result;
            }
        }
    }

    // Magic number
    result = tryFreeBlock(1024, size);
    if (result)
    {
#ifdef SWAG_DEV_MODE
        memset(result, 0xAA, size);
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
            if (g_CommandLine && g_CommandLine->stats)
                g_Stats.wastedMemory += remain;

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

        lastBucket = (AllocatorBucket*) malloc(sizeof(AllocatorBucket) + max(size, ALLOCATOR_BUCKET_SIZE));
        if (!lastBucket)
        {
            g_Log.error(Msg0014);
            OS::exit(-1);
            return nullptr;
        }

        lastBucket->maxUsed   = 0;
        lastBucket->allocated = max(size, ALLOCATOR_BUCKET_SIZE);
        lastBucket->data      = (uint8_t*) (lastBucket + 1);

        currentData = lastBucket->data;

        if (g_CommandLine && g_CommandLine->stats)
        {
            g_Stats.allocatorMemory += sizeof(AllocatorBucket);
            g_Stats.allocatorMemory += lastBucket->allocated;
            g_Stats.wastedMemory += lastBucket->allocated;
        }
    }

    auto returnData = currentData;
#ifdef SWAG_DEV_MODE
    memset(currentData, 0xAA, size);
#endif
    currentData += size;
    lastBucket->maxUsed += size;
    if (g_CommandLine && g_CommandLine->stats)
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

    if (g_CommandLine && g_CommandLine->stats)
        g_Stats.wastedMemory += size;

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

Allocator::Allocator()
{
    impl = &_impl;

    // Allocator created by the tls of a user bytecode thread. In that can, we use
    // the same shared AllocatorImpl
    if (g_CompilerAllocTh >= g_Stats.numWorkers && g_Stats.numWorkers)
    {
        shared = true;
        ScopedLock lk(g_AllocatorMutex);
        if (g_SharedAllocator)
        {
            impl = g_SharedAllocator->impl;
            SWAG_ASSERT(impl);
        }
        else
        {
            g_SharedAllocator = (Allocator*) malloc(sizeof(Allocator));
            if (!g_SharedAllocator)
            {
                g_Log.error(Msg0014);
                OS::exit(-1);
                return;
            }

            g_SharedAllocator->impl = &g_SharedAllocator->_impl;
            memset(g_SharedAllocator->impl, 0, sizeof(AllocatorImpl));
            impl = &g_SharedAllocator->_impl;
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
    Timer timer(&g_Stats.allocTime);

    if (shared)
        g_AllocatorMutex.lock();

#ifdef SWAG_CHECK_MEMORY
    auto userSize = size;
    size += 2 * sizeof(uint64_t);
#endif

    if (!impl)
        impl = &_impl;
    uint8_t* result = (uint8_t*) impl->alloc(size);

#ifdef SWAG_CHECK_MEMORY
    *(uint64_t*) result = MAGIC_ALLOC;
    result += sizeof(uint64_t);
    auto end         = result + userSize;
    *(uint64_t*) end = MAGIC_ALLOC;
#endif

    if (shared)
        g_AllocatorMutex.unlock();
    return result;
}

void Allocator::free(void* ptr, size_t size)
{
    if (!ptr || !size)
        return;

    Timer timer(&g_Stats.freeTime);

    if (shared)
        g_AllocatorMutex.lock();

    uint8_t* addr = (uint8_t*) ptr;
#ifdef SWAG_CHECK_MEMORY
    auto end = addr + size;
    SWAG_ASSERT(*(uint64_t*) end == MAGIC_ALLOC);
    addr -= sizeof(uint64_t);
    SWAG_ASSERT(*(uint64_t*) addr == MAGIC_ALLOC);
    size += 2 * sizeof(uint64_t);
#endif

    if (!impl)
        impl = &_impl;
    impl->free(addr, size);

    if (shared)
        g_AllocatorMutex.unlock();
}