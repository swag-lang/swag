#pragma once
struct BackendC;
struct BuildParameters;

struct BackendCCompiler
{
    BackendCCompiler(BackendC* bk)
        : backend{bk}
    {
    }

    virtual bool check()   = 0;
    virtual bool compile() = 0;

    string getResultFile();

    BackendC*              backend         = nullptr;
    const BuildParameters* buildParameters = nullptr;
};
