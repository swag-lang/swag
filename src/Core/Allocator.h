#pragma once
#include "Flags.h"

#define MK_ALIGN16(__s) \
    if ((__s) % 16)     \
    (__s) += 16 - ((__s) % 16)

using AllocFlags = Flags<uint32_t>;

constexpr AllocFlags ALLOC_STD = 0x00000001;
constexpr AllocFlags ALLOC_NEW = 0x00000002;

struct Allocator
{
    template<typename T>
    static T* allocRaw()
    {
        return static_cast<T*>(alloc(alignSize(sizeof(T))));
    }

    template<typename T>
    static T* alloc()
    {
        auto returnData = allocRaw<T>();
        ::new (returnData) T;
        return returnData;
    }

    template<typename T>
    static T* allocN(uint32_t n)
    {
        return static_cast<T*>(alloc(n * sizeof(T)));
    }

    template<typename T>
    static T* allocAlignedN(uint32_t n)
    {
        return static_cast<T*>(alloc(alignSize(n * sizeof(T))));
    }

    template<typename T>
    static void freeAlignedN(T* ptr, uint32_t n)
    {
        return free(ptr, alignSize(n * sizeof(T)));
    }

    template<typename T>
    static void free(void* ptr)
    {
        static_cast<T*>(ptr)->~T();
        free(ptr, alignSize(sizeof(T)));
    }

    static void* alloc(size_t size, size_t align = sizeof(void*), AllocFlags flags = 0);
    static void  free(void*, size_t size, AllocFlags flags = 0);

    static size_t   alignSize(size_t size) { return size + 7 & ~7; }
    static uint32_t alignSize(uint32_t size) { return size + 7 & ~7; }

#ifdef SWAG_CHECK_MEMORY
    static uint8_t* markDebugBlock(uint8_t* blockAddr, uint64_t userSize, uint64_t marker);
    static uint8_t* checkUserBlock(uint8_t* userAddr, uint64_t userSize, uint64_t marker);
#endif
};

template<typename T>
struct StdAllocator
{
    using value_type = T;

    template<class U>
    explicit constexpr StdAllocator(const StdAllocator<U>&) noexcept
    {
    }

    StdAllocator() = default;

    static T*   allocate(size_t n) { return static_cast<T*>(Allocator::alloc(Allocator::alignSize(n * sizeof(T)), sizeof(void*), ALLOC_STD)); }
    static void deallocate(T* p, size_t n) { Allocator::free(p, Allocator::alignSize(n * sizeof(T)), ALLOC_STD); }
};
