#pragma once
struct BackendC;

struct BackendCCompiler
{
    BackendCCompiler(BackendC* bk, const BuildParameters* params)
        : backend{bk}
        , buildParameters{params}
    {
    }

    virtual bool compile()  = 0;
    virtual bool runTests() = 0;

    BackendC*              backend;
    const BuildParameters* buildParameters;
};
