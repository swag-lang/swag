#pragma once
#include "BackendCCompiler.h"

struct BackendCCompilerWin32 : public BackendCCompiler
{
    BackendCCompilerWin32(BackendC* bk)
        : BackendCCompiler{bk}
    {
    }

    bool compile(const BuildParameters& buildParameters) override;
};
