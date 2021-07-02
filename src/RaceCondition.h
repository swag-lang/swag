#pragma once

struct RaceCondition
{
    struct Instance
    {
        shared_mutex     mutex;
        thread::id       lastThreadID;
        std::atomic<int> countWrite = 0;
        Instance()                  = default;
    };

    RaceCondition()                     = delete;
    RaceCondition(const RaceCondition&) = delete;
    RaceCondition(Instance* _instance, bool read);
    ~RaceCondition();

    bool      read = false;
    Instance* myInstance;
};

#ifdef SWAG_HAS_ASSERT
#define SWAG_RACE_CONDITION_WRITE(__x) RaceCondition rc(&__x, false);
#define SWAG_RACE_CONDITION_READ(__x) RaceCondition rc(&__x, true);
#define SWAG_RACE_CONDITION_INSTANCE(__x) RaceCondition::Instance __x;
#else
#define SWAG_RACE_CONDITION_WRITE(__x)
#define SWAG_RACE_CONDITION_READ(__x)
#define SWAG_RACE_CONDITION_INSTANCE(__x)
#endif