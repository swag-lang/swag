#pragma once
#include "Pool.h"

class Job : public PoolElement
{
    friend class ThreadManager;

public:
    virtual bool execute() = 0;
    void         reset() override{};

protected:
    class JobThread* m_thread = nullptr;
};
