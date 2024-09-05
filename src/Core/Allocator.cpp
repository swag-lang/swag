#include "pch.h"
#include "Core/Allocator.h"
#include "Extern/mimalloc/mimalloc.h"
#include "Main/Statistics.h"

#ifdef SWAG_HAS_MEMORY_CHECK
constexpr uint64_t MAGIC_ALLOC = 0xC0DEC0DEC0DEC0DE;
constexpr uint64_t MAGIC_FREE  = 0xCAFECAFECAFECAFE;
#endif

// ReSharper disable once CppParameterNamesMismatch
void* operator new(size_t size)
{
    size         = Allocator::alignSize(static_cast<int>(size) + 2 * sizeof(uint64_t));
    const auto p = static_cast<uint64_t*>(Allocator::alloc(size, 2 * sizeof(uint64_t), ALLOC_NEW));
    *p           = size;
    return p + 2;
}

// ReSharper disable once CppParameterNamesMismatch
void operator delete(void* block) noexcept
{
    if (!block)
        return;
    auto p = static_cast<uint64_t*>(block);
    p -= 2;
    Allocator::free(p, *p, ALLOC_NEW);
}

void* Allocator::alloc(size_t size, [[maybe_unused]] size_t align, [[maybe_unused]] AllocFlags flags)
{
#ifdef SWAG_HAS_MEMORY_CHECK
    auto result = mi_malloc_aligned(size + 3 * sizeof(uint64_t), align);
    result      = markDebugBlock(static_cast<uint8_t*>(result), size, MAGIC_ALLOC);
#else
    auto result = mi_malloc(size);
#endif

#ifdef SWAG_STATS
    g_Stats.allocatedMemory += size;
    if (flags.has(ALLOC_STD))
        g_Stats.memStd += size;
    if (flags.has(ALLOC_NEW))
        g_Stats.memNew += size;
    auto prevValue = g_Stats.maxAllocatedMemory.load();
    while (prevValue < g_Stats.allocatedMemory.load() && !g_Stats.maxAllocatedMemory.compare_exchange_weak(prevValue, g_Stats.allocatedMemory.load()))
    {
    }
#endif

    return result;
}

void Allocator::free(void* ptr, size_t size, [[maybe_unused]] AllocFlags flags)
{
    if (!ptr || !size)
        return;

#ifdef SWAG_HAS_MEMORY_CHECK
    ptr = checkUserBlock(static_cast<uint8_t*>(ptr), size, MAGIC_ALLOC);
    (void) markDebugBlock(static_cast<uint8_t*>(ptr), size, MAGIC_FREE);
#endif

#ifdef SWAG_STATS
    g_Stats.allocatedMemory -= size;
    if (flags.has(ALLOC_STD))
        g_Stats.memStd -= size;
    if (flags.has(ALLOC_NEW))
        g_Stats.memNew -= size;
#endif

    mi_free(ptr);
}

#ifdef SWAG_HAS_MEMORY_CHECK
uint8_t* Allocator::markDebugBlock(uint8_t* blockAddr, uint64_t userSize, uint64_t marker)
{
    memset(blockAddr + 2 * sizeof(uint64_t), static_cast<uint8_t>(marker), userSize);
    *reinterpret_cast<uint64_t*>(blockAddr) = marker;
    blockAddr += sizeof(uint64_t);
    *reinterpret_cast<uint64_t*>(blockAddr) = userSize;
    blockAddr += sizeof(uint64_t);
    *reinterpret_cast<uint64_t*>(blockAddr + userSize) = marker;
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
    SWAG_ASSERT(*reinterpret_cast<uint64_t*>(userAddr + userSize) == marker);
    return blockAddr;
}
#endif
