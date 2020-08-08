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

static void* iitf[] = {nullptr, (void*) itable};

void* getCompilerItf(Module* module)
{
    iitf[0] = module;
    return &iitf;
}