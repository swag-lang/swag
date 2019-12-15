#pragma once
struct BackendC;
struct BuildParameters;

struct BackendCCompiler
{
    BackendCCompiler(BackendC* bk, const BuildParameters* params)
        : backend{bk}
        , buildParameters{params}
    {
    }

    string getResultFile();
    bool   mustCompile();

    virtual bool compile() = 0;

    BackendC*              backend;
    const BuildParameters* buildParameters;
};
