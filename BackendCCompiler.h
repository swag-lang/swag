#pragma once
struct BackendC;

struct BackendCCompiler
{
    BackendCCompiler(BackendC* bk, const BuildParameters* params)
        : backend{bk}
        , buildParameters{params}
    {
    }

    virtual bool mustCompile() = 0;
    virtual bool compile()     = 0;

    BackendC*              backend;
    const BuildParameters* buildParameters;
};
