#pragma once
#include "Job.h"
#include "Concat.h"
#include "BuildParameters.h"
struct Module;
struct Backend;
struct ByteCode;

struct BackendFunctionBodyJob : public Job
{
    BackendFunctionBodyJob()
    {
        affinity = AFFINITY_ALL ^ AFFINITY_BACKEND_FCTBODY;
        jobKind  = JobKind::BACKEND_FCT_BODY;
    }

    void reset() override
    {
        Job::reset();
        backend         = nullptr;
        byteCodeFunc.clear();
    }

    BuildParameters         buildParameters;
    VectorNative<ByteCode*> byteCodeFunc;
    Backend*                backend         = nullptr;
};
