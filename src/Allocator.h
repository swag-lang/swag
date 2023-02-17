#pragma once
#include "Assert.h"
#include "Stats.h"
#define MAX_FREE_BUCKETS 512 / 8

struct AllocatorBlock
{
    uint8_t* data      = nullptr;
    size_t   maxUsed   = 0;
    size_t   allocated = 0;
};

struct Allocator
{
    void  allocateNewBlock(size_t size);
    void* bigAlloc(size_t size);
    void* allocBlock(size_t size);
    void  freeBlock(void*, size_t size);

    void* useRealBucket(uint32_t bucket, size_t size);
    void* useBucket(uint32_t bucket, size_t size);
    void* tryFreeBlock(uint32_t maxCount, size_t size);

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

    static void  allocAllocator();
    static void  free(void*, size_t size);
    static void* alloc(size_t size);

    // clang-format off
    static size_t alignSize(size_t size) { return ((size + 7) & ~7); }
    // clang-format on

#ifdef SWAG_CHECK_MEMORY
    static uint8_t* markDebugBlock(uint8_t* blockAddr, uint64_t userSize, uint64_t marker);
    static uint8_t* checkUserBlock(uint8_t* userAddr, uint64_t userSize, uint64_t marker);
#endif

    typedef struct FreeBlock
    {
        FreeBlock* next;
        size_t     size;
    } FreeBlock;

    FreeBlock*      firstFreeBlock                = nullptr;
    AllocatorBlock* lastBlock                     = nullptr;
    uint8_t*        currentData                   = nullptr;
    void*           freeBuckets[MAX_FREE_BUCKETS] = {0};
    uint64_t        freeBucketsMask               = 0;
    bool            shared                        = false;
};

extern atomic<int>            g_CompilerAllocTh;
extern thread_local Allocator *g_Allocator;

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
        return (T*) Allocator::alloc(Allocator::alignSize(n));
    }

    void deallocate(T* p, size_t n)
    {
        Allocator::free(p, Allocator::alignSize(n));
    }
};
