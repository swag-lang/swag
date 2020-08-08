#include "pch.h"
#include "Module.h"

void* getMessage(Module* module)
{
    return (void*) module->currentCompilerMessage;
}

using cb = void* (*) (Module*);

static cb itable[] =
    {
        getMessage,
};

static void* iitf[] = {nullptr, (void*) itable};

void* getCompilerItf(Module* module)
{
    iitf[0] = module;
    return &iitf;
}