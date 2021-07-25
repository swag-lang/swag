#pragma once
#include "Job.h"
#include "Concat.h"
#include "BackendParameters.h"
struct Module;
struct Backend;
struct ByteCode;

struct BackendFunctionBodyJob : public Job
{
    BackendFunctionBodyJob()
    {
        affinity = AFFINITY_ALL ^ AFFINITY_BACKEND_FCTBODY;
    }

    void reset() override
    {
        Job::reset();
        backend = nullptr;
        byteCodeFunc.clear();
    }

    BuildParameters         buildParameters;
    VectorNative<ByteCode*> byteCodeFunc;
    Backend*                backend = nullptr;
};
