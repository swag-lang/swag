#pragma once
#include "Threading/Mutex.h"

struct RaceCondition
{
    struct Instance
    {
        Instance() = default;

        SharedMutex      mutex;
        std::thread::id  lastThreadID;
        std::atomic<int> countWrite = 0;
    };

    RaceCondition() = default;
    RaceCondition(Instance* inst, bool read);
    ~RaceCondition();

    void lock(Instance* inst, bool r);
    void unlock() const;

    Instance* myInstance;

    bool read = false;
};

#define SWAG_RACE_CONDITION_ON_WRITE(__x)    RaceCondition rc(const_cast<RaceCondition::Instance*>(&(__x)), false)
#define SWAG_RACE_CONDITION_ON_WRITE1(__x)   RaceCondition rc1(const_cast<RaceCondition::Instance*>(&(__x)), false)
#define SWAG_RACE_CONDITION_ON_READ(__x)     RaceCondition rc(const_cast<RaceCondition::Instance*>(&(__x)), true)
#define SWAG_RACE_CONDITION_ON_READ1(__x)    RaceCondition rc1(const_cast<RaceCondition::Instance*>(&(__x)), true)
#define SWAG_RACE_CONDITION_ON_INSTANCE(__x) RaceCondition::Instance __x

#define SWAG_RACE_CONDITION_OFF_WRITE(__x) \
    do                                     \
    {                                      \
    } while (0)

#define SWAG_RACE_CONDITION_OFF_WRITE1(__x) \
    do                                      \
    {                                       \
    } while (0)

#define SWAG_RACE_CONDITION_OFF_READ(__x) \
    do                                    \
    {                                     \
    } while (0)

#define SWAG_RACE_CONDITION_OFF_READ1(__x) \
    do                                     \
    {                                      \
    } while (0)

#define SWAG_RACE_CONDITION_OFF_INSTANCE(__x) using __dummy##__x = int

#ifdef SWAG_HAS_RACE_CONDITION
#define SWAG_RACE_CONDITION_WRITE(__x)    SWAG_RACE_CONDITION_ON_WRITE(__x)
#define SWAG_RACE_CONDITION_WRITE1(__x)   SWAG_RACE_CONDITION_ON_WRITE1(__x)
#define SWAG_RACE_CONDITION_READ(__x)     SWAG_RACE_CONDITION_ON_READ(__x)
#define SWAG_RACE_CONDITION_READ1(__x)    SWAG_RACE_CONDITION_ON_READ1(__x)
#define SWAG_RACE_CONDITION_INSTANCE(__x) SWAG_RACE_CONDITION_ON_INSTANCE(__x)
#else
#define SWAG_RACE_CONDITION_WRITE(__x)    SWAG_RACE_CONDITION_OFF_WRITE(__x)
#define SWAG_RACE_CONDITION_WRITE1(__x)   SWAG_RACE_CONDITION_OFF_WRITE1(__x)
#define SWAG_RACE_CONDITION_READ(__x)     SWAG_RACE_CONDITION_OFF_READ(__x)
#define SWAG_RACE_CONDITION_READ1(__x)    SWAG_RACE_CONDITION_OFF_READ1(__x)
#define SWAG_RACE_CONDITION_INSTANCE(__x) SWAG_RACE_CONDITION_OFF_INSTANCE(__x)
#endif
