#pragma once
struct BackendC;
struct BuildParameters;

struct BackendCCompiler
{
    BackendCCompiler(BackendC* bk)
        : backend{bk}
    {
    }

    virtual bool compile(const BuildParameters& buildParameters) = 0;

    string getResultFile(const BuildParameters& buildParameters);

    BackendC* backend = nullptr;
};
