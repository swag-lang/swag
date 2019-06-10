#include "pch.h"
#include "Module.h"
#include "SourceFile.h"
#include "Global.h"
#include "ByteCodeRun.h"
#include "Workspace.h"
#include "Scope.h"
#include "Ast.h"
#include "TypeManager.h"

Pool<Module> g_Pool_module;

void Module::setup(Workspace* wkp, const fs::path& pth, bool runtime)
{
    path      = pth;
    workspace = wkp;

    if (runtime)
    {
        scopeRoot = workspace->scopeRoot;
    }
    else
    {
        scopeRoot       = g_Pool_scope.alloc();
        scopeRoot->kind = ScopeKind::Module;
        scopeRoot->allocateSymTable();
        scopeRoot->parentScope = workspace->scopeRoot;
    }

    astRoot             = Ast::newNode(&g_Pool_astNode, AstNodeKind::Module, UINT32_MAX);
    astRoot->ownerScope = workspace->scopeRoot;
}

void Module::addFile(SourceFile* file)
{
    scoped_lock lk(mutexFile);
    file->module        = this;
    file->indexInModule = (uint32_t) files.size();
    files.push_back(file);
}

void Module::removeFile(SourceFile* file)
{
    scoped_lock lk(mutexFile);
    assert(file->module == this);
    for (auto it = files.begin(); it != files.end(); ++it)
    {
        if (*it == file)
        {
            file->module        = nullptr;
            file->indexInModule = UINT32_MAX;
            files.erase(it);
            return;
        }
    }

    assert(false);
}

void Module::reserveRegisterRR(uint32_t count)
{
    scoped_lock lk(mutexRegisterRR);
    maxReservedRegisterRR = max(maxReservedRegisterRR, count);
}

uint32_t Module::reserveRegisterRC()
{
    scoped_lock lk(mutexRegisterRC);
    if (!availableRegistersRC.empty())
    {
        auto result = availableRegistersRC.back();
        availableRegistersRC.pop_back();
        return result;
    }

    auto result = maxReservedRegisterRC++;
    return result;
}

void Module::freeRegisterRC(uint32_t reg)
{
    scoped_lock lk(mutexRegisterRC);
    availableRegistersRC.push_back(reg);
}

bool Module::executeNode(SourceFile* sourceFile, AstNode* node)
{
    // Only one run at a time !
    static SpinLock mutex;
    {
        scoped_lock lk(mutex);
        auto        runContext = &workspace->runContext;
        runContext->setup(sourceFile, node, maxReservedRegisterRC, maxReservedRegisterRR, 1024);
        SWAG_CHECK(g_Run.run(runContext));

        if (node->resultRegisterRC != UINT32_MAX)
        {
            node->computedValue.reg = runContext->registersRC[node->resultRegisterRC];
            node->flags |= AST_VALUE_COMPUTED;
			node->typeInfo = TypeManager::concreteType(node->typeInfo);
        }
    }

    return true;
}