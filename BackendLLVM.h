#pragma once
#include "Backend.h"
struct Module;
struct BuildParameters;
struct Job;

struct BackendLLVM : public Backend
{
    BackendLLVM(Module* mdl)
        : Backend{mdl}
    {
    }

    bool      check() override;
    JobResult preCompile(Job* ownerJob, int preCompileIndex) override;
    bool      compile(const BuildParameters& backendParameters) override;
};