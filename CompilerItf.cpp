#include "pch.h"
#include "Module.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "ThreadManager.h"

void* getMessage(Module* module)
{
    return (void*) module->currentCompilerMessage;
}

void* getBuildCfg(Module* module)
{
    return (void*) &module->buildCfg;
}

void compileString(Module* module, const char* str, uint32_t count)
{
    if (!str || !count || !str[0])
        return;
    if (!module->currentCompilerMessageJob)
        return;
    Utf8 text;
    text.append(str, count);
    module->compileString(text, module->currentCompilerMessageJob);
}

using cb = void* (*) (Module*);

static cb itable[] = {
    (cb) getMessage,
    (cb) getBuildCfg,
    (cb) compileString,
};

void* getCompilerItf(Module* module)
{
    module->compilerItf[0] = module;
    module->compilerItf[1] = itable;
    return &module->compilerItf;
}