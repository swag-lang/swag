#include "pch.h"
#ifdef SWAG_HAS_ASSERT
#include "RaceCondition.h"
#include "Assert.h"

RaceCondition::RaceCondition(Instance* instance, bool read)
{
    scoped_lock lk(instance->mutex);
    myInstance = instance;

    auto currentThreadId = this_thread::get_id();
    SWAG_ASSERT(!myInstance->defined || myInstance->lastThreadID == currentThreadId || (myInstance->read && read));
    myInstance->count++;
    myInstance->lastThreadID = currentThreadId;
    myInstance->defined      = true;
    myInstance->read         = read;
}

RaceCondition::~RaceCondition()
{
    scoped_lock lk(myInstance->mutex);
    if (myInstance->lastThreadID == this_thread::get_id())
    {
        myInstance->count--;
        if (myInstance->count == 0)
            myInstance->defined = false;
    }
}

#endif