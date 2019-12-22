#pragma once
#define ALLOCATOR_BUCKET_SIZE 4 * 1024

struct AllocatorBucket
{
    uint8_t data[ALLOCATOR_BUCKET_SIZE];
    int     maxUsed = 0;
};

struct Allocator
{
    template<typename T>
    T* alloc()
    {
        static_assert(sizeof(T) < ALLOCATOR_BUCKET_SIZE);
        if (!lastBucket || lastBucket->maxUsed + sizeof(T) >= ALLOCATOR_BUCKET_SIZE)
        {
            lastBucket          = (AllocatorBucket*) malloc(sizeof(AllocatorBucket));
            lastBucket->maxUsed = 0;
            currentData         = lastBucket->data;
        }

        auto returnData = currentData;
        ::new (returnData) T;
        currentData += sizeof(T);
        lastBucket->maxUsed += sizeof(T);
        return (T*) returnData;
    }

    AllocatorBucket* lastBucket  = nullptr;
    uint8_t*         currentData = nullptr;
};

extern thread_local Allocator g_Allocator;