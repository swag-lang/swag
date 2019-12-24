#pragma once
struct BackendC;
struct BuildParameters;

struct BackendCCompiler
{
    BackendCCompiler(BackendC* bk)
        : backend{bk}
    {
    }

    string       getResultFile();
    virtual bool compile() = 0;

    BackendC*              backend         = nullptr;
    const BuildParameters* buildParameters = nullptr;
};
