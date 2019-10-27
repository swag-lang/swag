#pragma once
#include "BackendCCompiler.h"
#include "BuildParameters.h"

struct BackendCCompilerVS : public BackendCCompiler
{
    BackendCCompilerVS(BackendC* bk, const BuildParameters* params)
        : BackendCCompiler(bk, params)
    {
    }

    bool mustCompile() override;
    bool compile() override;

    string getResultFile();
    bool   getVSTarget(string& vsTarget);
    bool   getWinSdk(string& winSdk);
};
