#include "pch.h"
#include "RaceCondition.h"
#include "Assert.h"

RaceCondition::RaceCondition(Instance* instance, bool read)
{
    scoped_lock lk(instance->mutex);
    myInstance = instance;

    auto currentThreadId = this_thread::get_id();
    SWAG_ASSERT(!myInstance->countWrite || myInstance->lastThreadID == currentThreadId);
    this->read = read;

    if (!read)
    {
        myInstance->countWrite++;
        myInstance->lastThreadID = currentThreadId;
    }
}

RaceCondition::~RaceCondition()
{
    scoped_lock lk(myInstance->mutex);
    if (!read && myInstance->lastThreadID == this_thread::get_id())
    {
        SWAG_ASSERT(myInstance->countWrite);
        myInstance->countWrite--;
    }
}
