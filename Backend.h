#pragma once

struct Backend
{
    Backend(Module* mdl)
        : module{mdl}
    {
    }

    virtual bool compile()  = 0;
    virtual bool generate() = 0;

    Module* module;
};
