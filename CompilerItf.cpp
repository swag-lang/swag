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
    SyntaxJob syntaxJob;
    Utf8      text;
    text.append(str, count);

    AstNode* parent = Ast::newNode(module->files[0], AstNodeKind::StatementNoScope, module->files[0]->astRoot);
    if (!syntaxJob.constructEmbedded(text, parent, parent, CompilerAstKind::TopLevelInstruction))
        return;

    auto job          = g_Pool_semanticJob.alloc();
    job->sourceFile   = module->files[0];
    job->module       = module;
    job->dependentJob = module->currentCompilerMessageJob;
    job->nodes.push_back(parent);
    g_ThreadMgr.addJob(job);
    return;
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