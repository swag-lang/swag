#include "pch.h"
#include "RaceCondition.h"
#include "Assert.h"

RaceCondition::RaceCondition(Instance* instance, bool read)
{
    scoped_lock lk(instance->mutex);
    myInstance = instance;

    auto currentThreadId = this_thread::get_id();
    SWAG_ASSERT(!myInstance->defined || myInstance->lastThreadID == currentThreadId);
    myInstance->read = read;

    if (!read)
    {
        myInstance->count++;
        myInstance->lastThreadID = currentThreadId;
        myInstance->defined      = true;
    }
}

RaceCondition::~RaceCondition()
{
    scoped_lock lk(myInstance->mutex);
    if (!myInstance->read && myInstance->lastThreadID == this_thread::get_id())
    {
        myInstance->count--;
        if (myInstance->count == 0)
            myInstance->defined = false;
    }
}
