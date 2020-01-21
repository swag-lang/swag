#pragma once
#include "BackendCCompiler.h"

struct BackendCCompilerVS : public BackendCCompiler
{
    BackendCCompilerVS(BackendC* bk)
        : BackendCCompiler{bk}
    {
    }

    bool check() override;
    bool compile(const BuildParameters& buildParameters) override;
};
