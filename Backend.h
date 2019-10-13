#pragma once
struct BuildParameters;

struct Backend
{
    Backend(Module* mdl)
        : module{mdl}
    {
    }

    virtual bool generate()                                        = 0;
    virtual bool compile(const BuildParameters& backendParameters) = 0;

    Module* module;
};
