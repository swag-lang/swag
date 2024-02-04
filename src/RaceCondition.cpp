#include "pch.h"
#ifdef SWAG_HAS_ASSERT
#include "RaceCondition.h"
#include "Assert.h"
#include "Mutex.h"

RaceCondition::RaceCondition(Instance* inst, bool read)
{
    lock(inst, read);
}

void RaceCondition::lock(Instance* inst, bool r)
{
    ScopedLock lk(inst->mutex);
    myInstance = inst;

    const auto currentThreadId = this_thread::get_id();
    SWAG_ASSERT(!myInstance->countWrite || myInstance->lastThreadID == currentThreadId);
    read = r;

    if (!r)
    {
        ++myInstance->countWrite;
        myInstance->lastThreadID = currentThreadId;
    }
}

RaceCondition::~RaceCondition()
{
    unlock();
}

void RaceCondition::unlock() const
{
    ScopedLock lk(myInstance->mutex);
    if (!read && myInstance->lastThreadID == this_thread::get_id())
    {
        SWAG_ASSERT(myInstance->countWrite);
        --myInstance->countWrite;
    }
}

#endif
