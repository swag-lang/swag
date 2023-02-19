#include "pch.h"
#include "Allocator.h"
#include "CommandLine.h"
#include "Os.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Stats.h"
#include "ThreadManager.h"

const uint64_t ALLOCATOR_PAGE_SIZE = 1024 * 1024;
const uint64_t ALLOCATOR_BIG_ALLOC = 64 * 1024;

#ifdef SWAG_CHECK_MEMORY
const uint64_t MAGIC_ALLOC = 0xC0DEC0DEC0DEC0DE;
const uint64_t MAGIC_FREE  = 0xCAFECAFECAFECAFE;
#endif

atomic<uint32_t>        g_CompilerAllocTh = 0;
Mutex                   g_AllocatorMutex;
thread_local Allocator* g_Allocator       = nullptr;
Allocator*              g_SharedAllocator = nullptr;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

void* operator new(size_t t)
{
    t      = Allocator::alignSize((int) t + sizeof(uint64_t));
    auto p = (uint64_t*) Allocator::alloc(t);
    *p     = (uint64_t) t;
#ifdef SWAG_STATS
    g_Stats.memNew += t;
#endif
    return p + 1;
}

void operator delete(void* addr) noexcept
{
    if (!addr)
        return;
    auto p = (uint64_t*) addr;
    p--;
#ifdef SWAG_STATS
    g_Stats.memNew -= *p;
#endif
    return Allocator::free(p, *p);
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
    if (g_CompilerAllocTh >= g_ThreadMgr.numWorkers && g_ThreadMgr.numWorkers)
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
        if (!g_Allocator)
        {
            Report::error(Err(Fat0019));
            OS::exit(-1);
            return;
        }

        memset(g_Allocator, 0, sizeof(Allocator));
    }
}

void* Allocator::alloc(size_t size)
{
    allocAllocator();

    SWAG_ASSERT(!(size & 7));
    SWAG_ASSERT(g_Allocator);

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

    SWAG_ASSERT(!(size & 7));
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
    memset(blockAddr + 2 * sizeof(uint64_t), marker & 0xFF, userSize);
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

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

void Allocator::setFirstFreeBlock(AllocatorFreeBlock* block)
{
    if (firstFreeBlock && firstFreeBlock->size > block->size)
    {
        block->next          = firstFreeBlock->next;
        firstFreeBlock->next = block;
    }
    else
    {
        block->next    = firstFreeBlock;
        firstFreeBlock = block;
    }
}

void* Allocator::tryFreeBlock(size_t size)
{
    AllocatorFreeBlock* prevBlock = nullptr;
    auto                tryBlock  = firstFreeBlock;
    uint32_t            cpt       = 0;

    while (tryBlock && cpt++ < 1024)
    {
        SWAG_ASSERT(tryBlock->size >= ALLOCATOR_MAX_SIZE_BUCKETS);

        // This block does not have enough size. Skip it.
        if (tryBlock->size < size)
        {
            prevBlock = tryBlock;
            tryBlock  = tryBlock->next;
            continue;
        }

        // Unlink
        if (prevBlock)
            prevBlock->next = tryBlock->next;
        else
            firstFreeBlock = tryBlock->next;

        // The block does not have exactly the requested size.
        // We need to split it.
        auto remainSize = tryBlock->size - size;
        if (remainSize)
        {
            auto bucket = remainSize / 8;
            auto split  = (uint8_t*) tryBlock + size;

            // The remaining block can fit in a bucket.
            if (bucket < ALLOCATOR_MAX_FREE_BUCKETS)
            {
                auto next                     = freeBuckets[bucket];
                freeBuckets[bucket]           = split;
                *(void**) freeBuckets[bucket] = next;
            }

            // The remaining block is too big to fit.
            // Put it in the free block list
            else
            {
                auto freeBlock  = (AllocatorFreeBlock*) split;
                freeBlock->size = remainSize;
                setFirstFreeBlock(freeBlock);
            }
        }

#ifdef SWAG_STATS
        g_Stats.wastedMemory -= size;
#endif

        return tryBlock;
    }

    return nullptr;
}

void* Allocator::useRealBucket(uint32_t bucket, size_t size)
{
    SWAG_ASSERT(bucket < ALLOCATOR_MAX_FREE_BUCKETS);
    SWAG_ASSERT(freeBuckets[bucket]);

    auto result         = freeBuckets[bucket];
    freeBuckets[bucket] = *(void**) result;

#ifdef SWAG_STATS
    g_Stats.wastedMemory -= size;
#endif

    return result;
}

void Allocator::newPage(size_t size)
{
    // We get the remaining space in the last block, to be able to use it as
    // a free block
    if (lastBlock)
    {
        auto remain = lastBlock->allocated - lastBlock->maxUsed;
        if (remain)
        {
            SWAG_ASSERT(!(remain & 7));
            if (remain < ALLOCATOR_MAX_SIZE_BUCKETS)
            {
                auto bucket                   = remain / 8;
                auto next                     = freeBuckets[bucket];
                freeBuckets[bucket]           = lastBlock->data + lastBlock->maxUsed;
                *(void**) freeBuckets[bucket] = next;
            }
            else
            {
                auto freeBlock  = (AllocatorFreeBlock*) (lastBlock->data + lastBlock->maxUsed);
                freeBlock->size = remain;
                setFirstFreeBlock(freeBlock);
            }
        }
    }

    // Allocate a new block of datas
    auto allocated = max(size, ALLOCATOR_PAGE_SIZE);
    lastBlock      = (AllocatorPage*) malloc(sizeof(AllocatorPage) + allocated);
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

#ifdef SWAG_STATS
    g_Stats.allocatedMemory += sizeof(AllocatorPage) + lastBlock->allocated;
    g_Stats.wastedMemory += lastBlock->allocated;
#endif
}

void* Allocator::bigAlloc(size_t size)
{
    // Very big alloc, do a normal alloc
    if (size > ALLOCATOR_BIG_ALLOC)
    {
        auto result = std::malloc(size);
        if (!result)
        {
            Report::error(Err(Fat0019));
            OS::exit(-1);
        }

#ifdef SWAG_STATS
        g_Stats.allocatedMemory += size;
#endif
        return result;
    }

    // Do we need to allocate a new page ?
    if (!lastBlock || lastBlock->maxUsed + size > lastBlock->allocated)
    {
        // Try in the list of free blocks
        auto result = tryFreeBlock(size);
        if (result)
            return result;

        newPage(size);
    }

#ifdef SWAG_STATS
    g_Stats.wastedMemory -= size;
#endif

    // Allocate at the end of the last page
    currentData += size;
    lastBlock->maxUsed += size;
    return currentData - size;
}

void* Allocator::allocBlock(size_t size)
{
    if (size < ALLOCATOR_MAX_SIZE_BUCKETS)
    {
        // Try in the list of buckets
        auto bucket = size / 8;
        if (freeBuckets[bucket])
            return useRealBucket((uint32_t) bucket, size);
    }

    return bigAlloc(size);
}

void Allocator::freeBlock(void* ptr, size_t size)
{
    if (!ptr)
        return;

    // Very big alloc, do a normal alloc
    if (size > ALLOCATOR_BIG_ALLOC)
    {
        std::free(ptr);
#ifdef SWAG_STATS
        g_Stats.allocatedMemory -= size;
#endif
        return;
    }

#ifdef SWAG_STATS
    g_Stats.wastedMemory += size;
#endif

    if (size < ALLOCATOR_MAX_SIZE_BUCKETS)
    {
        auto bucket         = size / 8;
        *(void**) ptr       = freeBuckets[bucket];
        freeBuckets[bucket] = ptr;
    }
    else
    {
        auto freeBlock  = (AllocatorFreeBlock*) ptr;
        freeBlock->size = size;
        setFirstFreeBlock(freeBlock);
    }
}
