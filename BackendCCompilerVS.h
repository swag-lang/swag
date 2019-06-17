#pragma once
#include "BackendCCompiler.h"
#include "BackendParameters.h"

struct BackendCCompilerVS : public BackendCCompiler
{
    BackendCCompilerVS(BackendC* bk)
        : BackendCCompiler{bk}
    {
    }

    bool compile() override;
    bool runTests() override;

    bool getVSTarget(string& vsTarget);
    bool getWinSdk(string& winSdk);
    bool doProcess(const string& cmdline, const string& compilerPath, bool logAll);

    BackendParameters backendParameters;
};
