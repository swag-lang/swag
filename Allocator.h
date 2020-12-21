#pragma once
#include "Assert.h"
#include "Stats.h"
#define ALLOCATOR_BUCKET_SIZE 1024 * 1024
#define MAX_FREE_BUCKETS 256 / 8

struct AllocatorBucket
{
    uint8_t* data      = nullptr;
    size_t   maxUsed   = 0;
    size_t   allocated = 0;
};

struct Allocator
{
    Allocator();
    ~Allocator();

    template<typename T>
    T* alloc()
    {
        auto size       = alignSize(sizeof(T));
        auto returnData = alloc(size);
        ::new (returnData) T;
        return (T*) returnData;
    }

    template<typename T>
    T* alloc0()
    {
        auto size       = alignSize(sizeof(T));
        auto returnData = alloc(size);
        memset(returnData, 0, size);
        ::new (returnData) T;
        return (T*) returnData;
    }

    static size_t alignSize(size_t size);
    void          free(void*, size_t size);
    void*         alloc(size_t size);

    void* tryBucket(uint32_t bucket, size_t size);
    void* tryFreeBlock(uint32_t maxCount, size_t size);

    typedef struct FreeBlock
    {
        FreeBlock* next;
        size_t     size;
    } FreeBlock;

    Allocator*       nextFreeAllocator = nullptr;
    FreeBlock*       firstFreeBlock    = nullptr;
    AllocatorBucket* lastBucket        = nullptr;
    uint8_t*         currentData       = nullptr;
    void*            freeBuckets[MAX_FREE_BUCKETS];
    size_t           freeBucketsCpt[MAX_FREE_BUCKETS];
};

extern thread_local Allocator g_Allocator;