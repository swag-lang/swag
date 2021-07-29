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

    RaceCondition(){};
    RaceCondition(Instance* _instance, bool read);
    ~RaceCondition();
    void lock(Instance* _instance, bool read);
    void unlock();

    Instance* myInstance;

    bool read = false;
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