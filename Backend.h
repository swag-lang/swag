#pragma once
struct BackendParameters;

struct Backend
{
    Backend(Module* mdl)
        : module{mdl}
    {
    }

    virtual bool generate()                                          = 0;
    virtual bool compile(const BackendParameters& backendParameters) = 0;

    Module* module;
};
