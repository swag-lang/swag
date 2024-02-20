#include "pch.h"
#include "Context.h"
#include "Module.h"

void* getMessage(Module* module)
{
    return (void*) g_RunContext->currentCompilerMessage;
}

void* getBuildCfg(Module* module)
{
    return &module->buildCfg;
}

void compileString(Module* module, const char* str, uint32_t count)
{
    if (!str || !count || !str[0])
        return;
    Utf8 text;
    text.append(str, count);
    module->compileString(text);
}

namespace
{
    void* g_ItfTable[] = {
    reinterpret_cast<void*>(getMessage),
    reinterpret_cast<void*>(getBuildCfg),
    reinterpret_cast<void*>(compileString),
    };
}

void* getCompilerItf(Module* module)
{
    module->compilerItf[0] = module;
    module->compilerItf[1] = g_ItfTable;
    return &module->compilerItf;
}
