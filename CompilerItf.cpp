#include "pch.h"
#include "Module.h"

void* getMessage(Module* module)
{
    return (void*) module->currentCompilerMessage;
}

void* getBuildCfg(Module* module)
{
    return (void*) &module->buildCfg;
}

using cb = void* (*) (Module*);

static cb itable[] =
    {
        getMessage,
        getBuildCfg,
};

void* getCompilerItf(Module* module)
{
    module->compilerItf[0] = module;
    module->compilerItf[1] = itable;
    return &module->compilerItf;
}