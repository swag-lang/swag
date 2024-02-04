#pragma once
#include "Mutex.h"

#ifdef SWAG_HAS_ASSERT
struct RaceCondition
{
    struct Instance
    {
        Instance() = default;

        SharedMutex      mutex;
        thread::id       lastThreadID;
        std::atomic<int> countWrite = 0;
    };

    RaceCondition() = default;
    RaceCondition(Instance* inst, bool read);
    ~RaceCondition();
    void lock(Instance* inst, bool read);
    void unlock() const;

    Instance* myInstance;

    bool read = false;
};

#define SWAG_RACE_CONDITION_WRITE(__x) RaceCondition rc(const_cast<RaceCondition::Instance*>(&(__x)), false)
#define SWAG_RACE_CONDITION_WRITE1(__x) RaceCondition rc1(const_cast<RaceCondition::Instance*>(&(__x)), false)
#define SWAG_RACE_CONDITION_READ(__x) RaceCondition rc(const_cast<RaceCondition::Instance*>(&(__x)), true)
#define SWAG_RACE_CONDITION_READ1(__x) RaceCondition rc1(const_cast<RaceCondition::Instance*>(&(__x)), true)
#define SWAG_RACE_CONDITION_INSTANCE(__x) RaceCondition::Instance __x

#else
#define SWAG_RACE_CONDITION_WRITE(__x)
#define SWAG_RACE_CONDITION_WRITE1(__x)
#define SWAG_RACE_CONDITION_READ(__x)
#define SWAG_RACE_CONDITION_READ1(__x)
#define SWAG_RACE_CONDITION_INSTANCE(__x)
#endif
