#pragma once
#include "BackendCCompiler.h"

struct BackendCCompilerVcClang : public BackendCCompiler
{
    BackendCCompilerVcClang(BackendC* bk)
        : BackendCCompiler{bk}
    {
    }

    bool compile(const BuildParameters& buildParameters) override;
};
