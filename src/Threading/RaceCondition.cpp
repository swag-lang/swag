#include "pch.h"
#include "Report/Assert.h"
#include "Threading/Mutex.h"
#include "Threading/RaceCondition.h"

RaceCondition::RaceCondition(Instance* inst, bool read)
{
    lock(inst, read);
}

void RaceCondition::lock(Instance* inst, bool r)
{
    ScopedLock lk(inst->mutex);
    myInstance = inst;

    const auto currentThreadId = std::this_thread::get_id();
    SWAG_FORCE_ASSERT(!myInstance->countWrite || myInstance->lastThreadID == currentThreadId);
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
    if (!read && myInstance->lastThreadID == std::this_thread::get_id())
    {
        SWAG_FORCE_ASSERT(myInstance->countWrite);
        --myInstance->countWrite;
    }
}
