#include "pch.h"
#include "Allocator.h"
#include "mimalloc/mimalloc.h"

#ifdef SWAG_CHECK_MEMORY
constexpr uint64_t MAGIC_ALLOC = 0xC0DEC0DEC0DEC0DE;
constexpr uint64_t MAGIC_FREE  = 0xCAFECAFECAFECAFE;
#endif

void* operator new(size_t t)
{
    t = Allocator::alignSize((int) t + 2 * sizeof(uint64_t));

    const auto p = (uint64_t*) Allocator::alloc(t, 2 * sizeof(uint64_t));
    *p           = t;

#ifdef SWAG_STATS
    g_Stats.memNew += t;
#endif
    return p + 2;
}

void operator delete(void* addr) noexcept
{
    if (!addr)
        return;
    auto p = (uint64_t*) addr;
    p -= 2;
#ifdef SWAG_STATS
    g_Stats.memNew -= *p;
#endif
    return Allocator::free(p, *p);
}

void* Allocator::alloc(size_t size, size_t align)
{
#ifdef SWAG_CHECK_MEMORY
    auto result = mi_malloc_aligned(size + (3 * sizeof(uint64_t)), align);
    result      = markDebugBlock((uint8_t*) result, size, MAGIC_ALLOC);
#else
    auto result = mi_malloc(size);
#endif

#ifdef SWAG_STATS
    g_Stats.allocatedMemory += size;
    auto prevValue = g_Stats.maxAllocatedMemory.load();
    while (prevValue < g_Stats.allocatedMemory.load() && !g_Stats.maxAllocatedMemory.compare_exchange_weak(prevValue, g_Stats.allocatedMemory.load()));
#endif

    return result;
}

void Allocator::free(void* ptr, size_t size)
{
    if (!ptr || !size)
        return;

#ifdef SWAG_CHECK_MEMORY
    ptr = checkUserBlock((uint8_t*) ptr, size, MAGIC_ALLOC);
    markDebugBlock((uint8_t*) ptr, size, MAGIC_FREE);
#endif

#ifdef SWAG_STATS
    g_Stats.allocatedMemory -= size;
#endif

    mi_free(ptr);
}

#ifdef SWAG_CHECK_MEMORY
uint8_t* Allocator::markDebugBlock(uint8_t* blockAddr, uint64_t userSize, uint64_t marker)
{
    memset(blockAddr + 2 * sizeof(uint64_t), marker & 0xFF, userSize);
    *reinterpret_cast<uint64_t*>(blockAddr) = marker;
    blockAddr += sizeof(uint64_t);
    *reinterpret_cast<uint64_t*>(blockAddr) = userSize;
    blockAddr += sizeof(uint64_t);
    *(uint64_t*) (blockAddr + userSize) = marker;
    return blockAddr;
}

uint8_t* Allocator::checkUserBlock(uint8_t* userAddr, uint64_t userSize, uint64_t marker)
{
    userAddr -= 2 * sizeof(uint64_t);
    const auto blockAddr = userAddr;
    SWAG_ASSERT(*reinterpret_cast<uint64_t*>(userAddr) == marker);
    userAddr += sizeof(uint64_t);
    SWAG_ASSERT(*reinterpret_cast<uint64_t*>(userAddr) == userSize);
    userAddr += sizeof(uint64_t);
    SWAG_ASSERT(*(uint64_t*) (userAddr + userSize) == marker);
    return blockAddr;
}
#endif
