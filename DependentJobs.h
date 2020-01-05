#pragma once
#include "RaceCondition.h"
#include "VectorNative.h"

struct Job;

struct DependentJobs
{
    void add(Job* job);
    void clear();
    void setRunning();

    VectorNative<Job*> list;
    SWAG_RACE_CONDITION_INSTANCE(raceCondition);
};
