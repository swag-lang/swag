#pragma once
struct IPool
{
    virtual void free(void* addr) = 0;
};

struct PoolElement
{
    void release()
    {
        if (ownerPool)
            ownerPool->free(this);
    }

    virtual void reset()
    {
    }

    virtual void construct()
    {
        reset();
    }

    PoolElement* nextFree  = nullptr;
    IPool*       ownerPool = nullptr;
};

template<typename T, int S>
struct PoolSlot
{
    T         buffer[S];
    int       maxUsed  = 1;
    PoolSlot* nextSlot = nullptr;
};

template<typename T, int S = 4>
struct Pool : public IPool
{
    T* alloc()
    {
        lock_guard<mutex> lg(mutexBucket);
        if (firstFreeElem)
        {
            auto cur      = firstFreeElem;
            firstFreeElem = firstFreeElem->nextFree;
            cur->reset();
            return static_cast<T*>(cur);
        }

        if (!rootBucket)
        {
            rootBucket = lastBucket = new PoolSlot<T, S>();
            auto elem               = &lastBucket->buffer[0];
            elem->construct();
            elem->ownerPool = this;
            return elem;
        }

        if (lastBucket->maxUsed == S)
        {
            lastBucket->nextSlot = new PoolSlot<T, S>();
            lastBucket           = lastBucket->nextSlot;
            auto elem            = &lastBucket->buffer[0];
            elem->ownerPool      = this;
            elem->construct();
            return elem;
        }

        auto elem       = &lastBucket->buffer[lastBucket->maxUsed++];
        elem->ownerPool = this;
        elem->construct();
        return elem;
    }

    void free(void* addr)
    {
        lock_guard<mutex> lg(mutexBucket);
        ((T*) addr)->nextFree = firstFreeElem;
        firstFreeElem         = ((T*) addr);
    }

    PoolSlot<T, S>* rootBucket    = nullptr;
    PoolSlot<T, S>* lastBucket    = nullptr;
    PoolElement*    firstFreeElem = nullptr;
    mutex           mutexBucket;
};
