#pragma once
#include "Statistics.h"

#define MK_ALIGN16(__s) \
    if (__s % 16)       \
        __s += 16 - (__s % 16);

struct Allocator
{
    template<typename T>
    static T* allocRaw()
    {
        return (T*) alloc(alignSize(sizeof(T)));
    }

    template<typename T>
    static T* alloc()
    {
        auto returnData = allocRaw<T>();
        ::new (returnData) T;
        return returnData;
    }

    template<typename T>
    static void free(void* ptr)
    {
        ((T*) ptr)->~T();
        free(ptr, alignSize(sizeof(T)));
    }

    static void* alloc(size_t size, size_t align = sizeof(void*));
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
