#pragma once
#include "BackendCCompiler.h"

struct BackendCCompilerVS : public BackendCCompiler
{
    BackendCCompilerVS(BackendC* bk)
        : BackendCCompiler{bk}
    {
    }

    bool compile() override;
};
