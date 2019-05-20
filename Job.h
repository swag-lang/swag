#pragma once
#include "Pool.h"

struct Job : public PoolElement
{
    virtual bool execute() = 0;
    void         reset() override{};

    struct JobThread* m_thread = nullptr;
};
