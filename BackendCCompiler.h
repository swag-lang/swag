#pragma once
struct BackendC;

struct BackendCCompiler
{
    BackendCCompiler(BackendC* bk)
        : backend{bk}
    {
    }

    virtual bool compile() = 0;

    BackendC* backend;
};
