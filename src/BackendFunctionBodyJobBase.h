#pragma once
#include "Job.h"
#include "Concat.h"
#include "BackendParameters.h"
struct Module;
struct Backend;
struct ByteCode;

struct BackendFunctionBodyJobBase : public Job
{
    BuildParameters         buildParameters;
    VectorNative<ByteCode*> byteCodeFunc;
    Backend*                backend = nullptr;
};
