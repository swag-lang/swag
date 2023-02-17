#include "pch.h"
#include "Allocator.h"
#include "CommandLine.h"
#include "Os.h"
#include "ErrorIds.h"
#include "Report.h"

const uint64_t ALLOCATOR_BLOCK_SIZE = 1024 * 1024;

#ifdef SWAG_CHECK_MEMORY
const uint64_t MAGIC_ALLOC = 0xC0DEC0DEC0DEC0DE;
const uint64_t MAGIC_FREE  = 0xCAFECAFECAFECAFE;
#endif

atomic<int>             g_CompilerAllocTh = 0;
Mutex                   g_AllocatorMutex;
thread_local Allocator* g_Allocator       = nullptr;
Allocator*              g_SharedAllocator = nullptr;

void* operator new(size_t t)
{
    t      = Allocator::alignSize((int) t + sizeof(uint64_t));
    auto p = (uint64_t*) Allocator::alloc(t);
    *p     = (uint64_t) t;
    if (g_CommandLine.stats)
        g_Stats.memNew += t;
    return p + 1;
}

void operator delete(void* addr) noexcept
{
    if (!addr)
        return;
    auto p = (uint64_t*) addr;
    p--;
    if (g_CommandLine.stats)
        g_Stats.memNew -= *p;
    return Allocator::free(p, *p);
}

void* Allocator::tryFreeBlock(uint32_t maxCount, size_t size)
{
    FreeBlock* prevBlock = nullptr;
    auto       tryBlock  = firstFreeBlock;
    uint32_t   cpt       = 0;

    while (tryBlock && cpt++ < maxCount)
    {
        // This block does not have enough size. Skip it.
        if (size > tryBlock->size)
        {
            prevBlock = tryBlock;
            tryBlock  = tryBlock->next;
            SWAG_ASSERT(tryBlock != firstFreeBlock);
            continue;
        }

        auto splitBlock = tryBlock->next;
        auto remainSize = tryBlock->size - size;

        // The block does not have exactly the requested size.
        // We need to split it.
        if (remainSize)
        {
            auto bucket = remainSize / 8;
            auto split  = (uint8_t*) tryBlock + size;

            // The remaining block can fit in a bucket.
            if (bucket < MAX_FREE_BUCKETS)
            {
                auto next           = freeBuckets[bucket];
                freeBuckets[bucket] = split;
                freeBucketsMask |= 1ULL << bucket;
                *(void**) freeBuckets[bucket] = next;
            }

            // The remaining block is too big to fit.
            // Put it in the free block list
            else
            {
                splitBlock       = (FreeBlock*) split;
                splitBlock->size = remainSize;
                splitBlock->next = tryBlock->next;
            }
        }

        // The block has exactly the requested size. Just take it.
        if (g_CommandLine.stats)
            g_Stats.wastedMemory -= size;
        if (prevBlock)
            prevBlock->next = splitBlock;
        else
            firstFreeBlock = splitBlock;
        return tryBlock;
    }

    return nullptr;
}

void* Allocator::useRealBucket(uint32_t bucket, size_t size)
{
    SWAG_ASSERT(bucket < MAX_FREE_BUCKETS);
    SWAG_ASSERT(freeBuckets[bucket]);

    if (g_CommandLine.stats)
        g_Stats.wastedMemory -= bucket * 8;
    auto result         = freeBuckets[bucket];
    freeBuckets[bucket] = *(void**) result;
    if (!freeBuckets[bucket])
        freeBucketsMask &= ~(1ULL << bucket);
#ifdef SWAG_DEV_MODE
    memset(result, 0xAA, size);
#endif
    return result;
}

void* Allocator::useBucket(uint32_t bucket, size_t size)
{
    SWAG_ASSERT(bucket < MAX_FREE_BUCKETS);
    SWAG_ASSERT(freeBuckets[bucket]);

    // If the bucket size is greater than the real size, then put the remaining
    // memory size in the corresponding bucket
    auto wasted = (bucket * 8) - size;
    SWAG_ASSERT(wasted);
    auto wastedBucket         = wasted / 8;
    auto ptr                  = (int8_t*) freeBuckets[bucket] + size;
    *(void**) ptr             = freeBuckets[wastedBucket];
    freeBuckets[wastedBucket] = ptr;
    freeBucketsMask |= 1ULL << wastedBucket;
    if (g_CommandLine.stats)
        g_Stats.wastedMemory += wasted;

    return useRealBucket(bucket, size);
}

void Allocator::allocateNewBlock(size_t size)
{
    // We get the remaining space in the last block, to be able to use it as
    // a free block
    if (lastBlock)
    {
        auto remain = lastBlock->allocated - lastBlock->maxUsed;
        if (remain)
        {
            SWAG_ASSERT(!(remain & 7));

            auto bucket = remain / 8;
            if (bucket < MAX_FREE_BUCKETS)
            {
                auto next                     = freeBuckets[bucket];
                freeBuckets[bucket]           = lastBlock->data + lastBlock->maxUsed;
                *(void**) freeBuckets[bucket] = next;
                freeBucketsMask |= 1ULL << bucket;
            }
            else
            {
                auto next            = firstFreeBlock;
                firstFreeBlock       = (FreeBlock*) (lastBlock->data + lastBlock->maxUsed);
                firstFreeBlock->size = remain;
                firstFreeBlock->next = next;
            }
        }
    }

    // Allocate a new block of datas
    auto allocated = max(size, ALLOCATOR_BLOCK_SIZE);
    lastBlock      = (AllocatorBlock*) malloc(sizeof(AllocatorBlock) + allocated);
    if (!lastBlock)
    {
        Report::error(Err(Fat0019));
        OS::exit(-1);
        return;
    }

    lastBlock->maxUsed   = 0;
    lastBlock->allocated = allocated;
    lastBlock->data      = (uint8_t*) (lastBlock + 1);

    currentData = lastBlock->data;

    if (g_CommandLine.stats)
    {
        g_Stats.allocatorMemory += sizeof(AllocatorBlock) + lastBlock->allocated;
        g_Stats.wastedMemory += lastBlock->allocated;
    }
}

void* Allocator::bigAlloc(size_t size)
{
    if (firstFreeBlock)
    {
        auto result = tryFreeBlock(1024, size);
        if (result)
        {
#ifdef SWAG_DEV_MODE
            memset(result, 0xAA, size);
#endif
            return result;
        }
    }

    // Do we need to allocate a new data block ?
    if (!lastBlock || lastBlock->maxUsed + size > lastBlock->allocated)
    {
        allocateNewBlock(size);
    }

#ifdef SWAG_DEV_MODE
    memset(currentData, 0xAA, size);
#endif

    currentData += size;
    lastBlock->maxUsed += size;
    if (g_CommandLine.stats)
        g_Stats.wastedMemory -= size;
    return currentData - size;
}

void* Allocator::allocBlock(size_t size)
{
    SWAG_ASSERT((size & 7) == 0);

    auto bucket = size / 8;
    if (bucket < MAX_FREE_BUCKETS)
    {
        // Try in the list of free blocks, per bucket
        if (freeBuckets[bucket])
            return useRealBucket((uint32_t) bucket, size);

        // Try with the biggest bucket available with free blocks
        // This will split the block
        uint64_t mask = ~((1ULL << bucket) - 1) & freeBucketsMask;
        if (mask)
        {
            bucket = 63 - OS::bitcountlz(mask);
            return useBucket((uint32_t) bucket, size);
        }
    }

    return bigAlloc(size);
}

void Allocator::freeBlock(void* ptr, size_t size)
{
    if (!ptr)
        return;
    SWAG_ASSERT(!(size & 7));

    if (g_CommandLine.stats)
        g_Stats.wastedMemory += size;

#ifdef SWAG_DEV_MODE
    memset(ptr, 0xFE, size);
#endif

    auto bucket = size / 8;
    if (bucket < MAX_FREE_BUCKETS)
    {
        *(void**) ptr       = freeBuckets[bucket];
        freeBuckets[bucket] = ptr;
        freeBucketsMask |= 1ULL << bucket;
    }
    else
    {
        auto fptr      = (FreeBlock*) ptr;
        fptr->size     = size;
        fptr->next     = firstFreeBlock;
        firstFreeBlock = fptr;
    }
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

void Allocator::allocAllocator()
{
    if (g_Allocator)
        return;

    // Allocator created by the tls of a user bytecode thread. In that case, we use
    // the same shared AllocatorImpl
    if (g_CompilerAllocTh >= g_Stats.numWorkers && g_Stats.numWorkers)
    {
        ScopedLock lk(g_AllocatorMutex);
        if (!g_SharedAllocator)
        {
            g_SharedAllocator = (Allocator*) malloc(sizeof(Allocator));
            if (!g_SharedAllocator)
            {
                Report::error(Err(Fat0019));
                OS::exit(-1);
                return;
            }

            memset(g_SharedAllocator, 0, sizeof(Allocator));
            g_SharedAllocator->shared = true;
        }

        g_Allocator = g_SharedAllocator;
    }
    else
    {
        g_Allocator = (Allocator*) malloc(sizeof(Allocator));
        memset(g_Allocator, 0, sizeof(Allocator));
    }
}

void* Allocator::alloc(size_t size)
{
    allocAllocator();
    if (g_Allocator->shared)
        g_AllocatorMutex.lock();

#ifdef SWAG_CHECK_MEMORY
    auto result = g_Allocator->allocBlock(size + (3 * sizeof(uint64_t)));
    result      = markDebugBlock((uint8_t*) result, size, MAGIC_ALLOC);
#else
    auto result = g_Allocator->allocBlock(size);
#endif

    if (g_Allocator->shared)
        g_AllocatorMutex.unlock();
    return result;
}

void Allocator::free(void* ptr, size_t size)
{
    if (!ptr || !size)
        return;

    SWAG_ASSERT(g_Allocator);
    if (g_Allocator->shared)
        g_AllocatorMutex.lock();

#ifdef SWAG_CHECK_MEMORY
    ptr = checkUserBlock((uint8_t*) ptr, size, MAGIC_ALLOC);
    markDebugBlock((uint8_t*) ptr, size, MAGIC_FREE);
#endif

    g_Allocator->freeBlock(ptr, size);

    if (g_Allocator->shared)
        g_AllocatorMutex.unlock();
}

#ifdef SWAG_CHECK_MEMORY
uint8_t* Allocator::markDebugBlock(uint8_t* blockAddr, uint64_t userSize, uint64_t marker)
{
    *(uint64_t*) blockAddr = marker;
    blockAddr += sizeof(uint64_t);
    *(uint64_t*) blockAddr = userSize;
    blockAddr += sizeof(uint64_t);
    *(uint64_t*) (blockAddr + userSize) = marker;
    return blockAddr;
}

uint8_t* Allocator::checkUserBlock(uint8_t* userAddr, uint64_t userSize, uint64_t marker)
{
    userAddr -= 2 * sizeof(uint64_t);
    auto blockAddr = userAddr;
    SWAG_ASSERT(*(uint64_t*) userAddr == marker);
    userAddr += sizeof(uint64_t);
    SWAG_ASSERT(*(uint64_t*) userAddr == userSize);
    userAddr += sizeof(uint64_t);
    SWAG_ASSERT(*(uint64_t*) (userAddr + userSize) == marker);
    return blockAddr;
}
#endif