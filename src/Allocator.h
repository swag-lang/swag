#pragma once
#include "Statistics.h"
struct Allocator
{
    template<typename T>
    static T* alloc()
    {
        auto size       = alignSize(sizeof(T));
        auto returnData = alloc(size);
        ::new (returnData) T;
        return (T*) returnData;
    }

    template<typename T>
    static void free(void* ptr)
    {
        ((T*) ptr)->~T();
        free(ptr, alignSize(sizeof(T)));
    }

    static void* alloc(size_t size);
    static void  free(void*, size_t size);

    // clang-format off
    static size_t alignSize(size_t size) { return ((size + 7) & ~7); }
    // clang-format on

#ifdef SWAG_CHECK_MEMORY
    static uint8_t* markDebugBlock(uint8_t* blockAddr, uint64_t userSize, uint64_t marker);
    static uint8_t* checkUserBlock(uint8_t* userAddr, uint64_t userSize, uint64_t marker);
#endif
};

template<typename T>
struct StdAllocator
{
    typedef T value_type;
    template<class U>
    constexpr StdAllocator(const StdAllocator<U>&) noexcept
    {
    }

    StdAllocator() = default;

    T* allocate(size_t n, const T* hint = 0)
    {
        auto size = Allocator::alignSize(n * sizeof(T));
#ifdef SWAG_STATS
        g_Stats.memStd += size;
#endif
        return (T*) Allocator::alloc(size);
    }

    void deallocate(T* p, size_t n)
    {
        auto size = n * sizeof(T);
#ifdef SWAG_STATS
        g_Stats.memStd -= size;
#endif
        Allocator::free(p, Allocator::alignSize(size));
    }
};
