#include "pch.h"
#include "Module.h"
#include "SourceFile.h"
#include "Pool.h"
#include "PoolFactory.h"
#include "Global.h"
#include "SemanticJob.h"
#include "ThreadManager.h"
#include "SymTable.h"

Module::Module(const fs::path& path)
    : path{path}
{
    reset();

    scopeRoot       = poolFactory.scope.alloc();
    scopeRoot->kind = ScopeKind::Module;
    scopeRoot->allocateSymTable();

    astRoot = Ast::newNode(&poolFactory.astNode, AstNodeKind::Module, nullptr, UINT32_MAX);
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
