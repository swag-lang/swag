#pragma once
struct PoolElement
{
};

template<typename T, int S>
struct PoolSlot
{
    T         buffer[S];
    int       maxUsed  = 1;
    PoolSlot* nextSlot = nullptr;
};

template<typename T, int S = 32>
struct Pool
{
    T* alloc()
    {
        unique_lock lk(mutexBucket);
        if (!rootBucket)
        {
            rootBucket = lastBucket = new PoolSlot<T, S>();
            auto elem               = &lastBucket->buffer[0];
            elem->reset();
            return elem;
        }

        if (lastBucket->maxUsed == S)
        {
            lastBucket->nextSlot = new PoolSlot<T, S>();
            lastBucket           = lastBucket->nextSlot;
            auto elem            = &lastBucket->buffer[0];
            elem->reset();
            return elem;
        }

        auto elem = &lastBucket->buffer[lastBucket->maxUsed++];
        elem->reset();
        return elem;
    }

    PoolSlot<T, S>* rootBucket = nullptr;
    PoolSlot<T, S>* lastBucket = nullptr;
    shared_mutex    mutexBucket;
};
