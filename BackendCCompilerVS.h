#pragma once
#include "BackendCCompiler.h"
struct BackendParameters;

struct BackendCCompilerVS : public BackendCCompiler
{
    BackendCCompilerVS(BackendC* bk, const BackendParameters& params)
        : BackendCCompiler{bk}
        , backendParameters{params}
    {
    }

    bool compile() override;
    bool runTests() override;

    bool getVSTarget(string& vsTarget);
    bool getWinSdk(string& winSdk);
    bool doProcess(const string& cmdline, const string& compilerPath, bool logAll);

    const BackendParameters& backendParameters;
};
