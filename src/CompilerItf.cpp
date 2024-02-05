#include "pch.h"
#include "Context.h"
#include "Module.h"

void* getMessage(Module* module)
{
    return (void*) g_RunContext->currentCompilerMessage;
}

void* getBuildCfg(Module* module)
{
    return (void*) &module->buildCfg;
}

void compileString(Module* module, const char* str, uint32_t count)
{
    if (!str || !count || !str[0])
        return;
    Utf8 text;
    text.append(str, count);
    module->compileString(text);
}

using cb = void* (*)(Module*);

namespace
{
    cb itable[] = {
        (cb) getMessage,
        (cb) getBuildCfg,
        (cb) compileString,
    };
}

void* getCompilerItf(Module* module)
{
    module->compilerItf[0] = module;
    module->compilerItf[1] = itable;
    return &module->compilerItf;
}
