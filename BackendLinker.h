#pragma once
struct BackendC;
struct BuildParameters;

struct BackendLinker
{
    BackendLinker(Backend* bk)
        : backend{bk}
    {
    }

    virtual bool link(const BuildParameters& buildParameters) = 0;
    Backend*     backend                                      = nullptr;
};
