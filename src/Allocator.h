#pragma once
#include "Assert.h"
#include "Stats.h"
#define MAX_FREE_BUCKETS 256 / 8

struct AllocatorBucket
{
    uint8_t* data      = nullptr;
    size_t   maxUsed   = 0;
    size_t   allocated = 0;
};

struct AllocatorImpl
{
    AllocatorImpl();

    void  free(void*, size_t size);
    void* alloc(size_t size);

    void* useBucket(uint32_t bucket, size_t size);
    void* tryFreeBlock(uint32_t maxCount, size_t size);

    typedef struct FreeBlock
    {
        FreeBlock* next;
        size_t     size;
    } FreeBlock;

    FreeBlock*       firstFreeBlock = nullptr;
    AllocatorBucket* lastBucket     = nullptr;
    uint8_t*         currentData    = nullptr;
    void*            freeBuckets[MAX_FREE_BUCKETS];
};

struct Allocator
{
    Allocator();

    template<typename T>
    T* alloc()
    {
        auto size       = alignSize(sizeof(T));
        auto returnData = alloc(size);
        ::new (returnData) T;
        return (T*) returnData;
    }

    template<typename T>
    void free(void* ptr)
    {
        ((T*) ptr)->~T();
        free(ptr, alignSize(sizeof(T)));
    }

    void  free(void*, size_t size);
    void* alloc(size_t size);

    static size_t alignSize(size_t size);
#ifdef SWAG_CHECK_MEMORY
    static void checkBlock(void* ptr);
#endif

    AllocatorImpl* impl = nullptr;
    AllocatorImpl  _impl;
    bool           shared = false;
};

extern atomic<int>            g_CompilerAllocTh;
extern thread_local Allocator g_Allocator;

#ifdef SWAG_CHECK_MEMORY
#define SWAG_CHECK_BLOCK(__addr) Allocator::checkBlock(__addr);
#else
#define SWAG_CHECK_BLOCK(__addr)
#endif
