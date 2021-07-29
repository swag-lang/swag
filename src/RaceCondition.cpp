#include "pch.h"
#ifdef SWAG_HAS_ASSERT
#include "RaceCondition.h"
#include "Assert.h"

RaceCondition::RaceCondition(Instance* instance, bool read)
{
    lock(instance, read);
}

void RaceCondition::lock(Instance* instance, bool r)
{
    scoped_lock lk(instance->mutex);
    myInstance = instance;

    auto currentThreadId = this_thread::get_id();
    SWAG_ASSERT(!myInstance->countWrite || myInstance->lastThreadID == currentThreadId);
    read = r;

    if (!r)
    {
        myInstance->countWrite++;
        myInstance->lastThreadID = currentThreadId;
    }
}

RaceCondition::~RaceCondition()
{
    unlock();
}

void RaceCondition::unlock()
{
    scoped_lock lk(myInstance->mutex);
    if (!read && myInstance->lastThreadID == this_thread::get_id())
    {
        SWAG_ASSERT(myInstance->countWrite);
        myInstance->countWrite--;
    }
}

#endif