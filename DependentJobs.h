#pragma once
#include "RaceCondition.h"
struct Job;

struct DependentJobs
{
    void add(Job* job);
    void clear();
    void setRunning();

    vector<Job*> list;
    SWAG_RACE_CONDITION_INSTANCE(raceCondition);
};
