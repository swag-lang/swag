#pragma once
template<typename T, int S>
struct PoolSlot
{
    T         buffer[S];
    PoolSlot* nextSlot = nullptr;
    int       maxUsed  = 1;
};

template<typename T, int S = 32>
struct Pool
{
    T* alloc()
    {
        if (!rootBucket)
        {
            rootBucket = lastBucket = new PoolSlot<T, S>();
            return &lastBucket->buffer[0];
        }

        if (lastBucket->maxUsed == S)
        {
            lastBucket->nextSlot = new PoolSlot<T, S>();
            lastBucket           = lastBucket->nextSlot;
            return &lastBucket->buffer[0];
        }

        return &lastBucket->buffer[lastBucket->maxUsed++];
    }

    PoolSlot<T, S>* rootBucket = nullptr;
    PoolSlot<T, S>* lastBucket = nullptr;
};
