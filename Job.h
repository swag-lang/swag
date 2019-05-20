#pragma once
#include "Pool.h"
struct JobThread;

struct Job : public PoolElement
{
    virtual bool execute() = 0;
    void         reset() override{};

    JobThread* thread = nullptr;
};
