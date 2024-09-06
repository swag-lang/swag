#pragma once
#include "Threading/Mutex.h"

#if defined(SWAG_HAS_RACE_CONDITION) || defined(SWAG_HAS_RACE_CONDITION_CORE)
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
#endif

#ifdef SWAG_HAS_RACE_CONDITION

#define SWAG_RACE_CONDITION_WRITE(__x)    RaceCondition rc(const_cast<RaceCondition::Instance*>(&(__x)), false)
#define SWAG_RACE_CONDITION_WRITE1(__x)   RaceCondition rc1(const_cast<RaceCondition::Instance*>(&(__x)), false)
#define SWAG_RACE_CONDITION_READ(__x)     RaceCondition rc(const_cast<RaceCondition::Instance*>(&(__x)), true)
#define SWAG_RACE_CONDITION_READ1(__x)    RaceCondition rc1(const_cast<RaceCondition::Instance*>(&(__x)), true)
#define SWAG_RACE_CONDITION_INSTANCE(__x) RaceCondition::Instance __x

#else
#define SWAG_RACE_CONDITION_WRITE(__x) \
    do                                 \
    {                                  \
    } while (0)
#define SWAG_RACE_CONDITION_WRITE1(__x) \
    do                                  \
    {                                   \
    } while (0)
#define SWAG_RACE_CONDITION_READ(__x) \
    do                                \
    {                                 \
    } while (0)
#define SWAG_RACE_CONDITION_READ1(__x) \
    do                                 \
    {                                  \
    } while (0)
#define SWAG_RACE_CONDITION_INSTANCE(__x) using __dummy##__x = int
#endif

#ifdef SWAG_HAS_RACE_CONDITION_CORE

#define SWAG_RACE_CONDITION_WRITE_CORE(__x)    RaceCondition rc(const_cast<RaceCondition::Instance*>(&(__x)), false)
#define SWAG_RACE_CONDITION_WRITE1_CORE(__x)   RaceCondition rc1(const_cast<RaceCondition::Instance*>(&(__x)), false)
#define SWAG_RACE_CONDITION_READ_CORE(__x)     RaceCondition rc(const_cast<RaceCondition::Instance*>(&(__x)), true)
#define SWAG_RACE_CONDITION_READ1_CORE(__x)    RaceCondition rc1(const_cast<RaceCondition::Instance*>(&(__x)), true)
#define SWAG_RACE_CONDITION_INSTANCE_CORE(__x) RaceCondition::Instance __x

#else

#define SWAG_RACE_CONDITION_WRITE_CORE(__x) \
    do                                      \
    {                                       \
    } while (0)
#define SWAG_RACE_CONDITION_WRITE1_CORE(__x) \
    do                                       \
    {                                        \
    } while (0)
#define SWAG_RACE_CONDITION_READ_CORE(__x) \
    do                                     \
    {                                      \
    } while (0)
#define SWAG_RACE_CONDITION_READ1_CORE(__x) \
    do                                      \
    {                                       \
    } while (0)
#define SWAG_RACE_CONDITION_INSTANCE_CORE(__x) using __dummy##__x = int
#endif
