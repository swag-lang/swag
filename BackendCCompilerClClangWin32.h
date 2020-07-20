#pragma once
#include "BackendCCompiler.h"

struct BackendCCompilerClClangWin32 : public BackendCCompiler
{
    BackendCCompilerClClangWin32(BackendC* bk)
        : BackendCCompiler{bk}
    {
    }

    bool compile(const BuildParameters& buildParameters) override;
};
