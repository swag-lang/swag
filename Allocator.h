#pragma once
#include "Assert.h"
#include "Stats.h"
#define ALLOCATOR_BUCKET_SIZE 128 * 1024
#define MAX_FREE_BUCKETS 1024 / 8

struct AllocatorBucket
{
    uint8_t* data      = nullptr;
    int      maxUsed   = 0;
    int      allocated = 0;
    bool     isReused  = false;
};

struct Allocator
{
    Allocator()
    {
        memset(freeBuckets, 0, sizeof(freeBuckets));
    }

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

    int   alignSize(int size);
    void  free(void*, int size);
    void* alloc(int size);

    void* tryFreeBlock(uint32_t maxCount, int size);

    typedef struct FreeBlock
    {
        FreeBlock* next;
        int        size;
    } FreeBlock;

    FreeBlock*       firstFreeBlock = nullptr;
    AllocatorBucket* lastBucket     = nullptr;
    uint8_t*         currentData    = nullptr;
    void*            freeBuckets[MAX_FREE_BUCKETS];
};

extern thread_local Allocator g_Allocator;