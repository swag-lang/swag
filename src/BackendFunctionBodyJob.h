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

    BuildParameters         buildParameters;
    VectorNative<ByteCode*> byteCodeFunc;
    Backend*                backend = nullptr;
};
