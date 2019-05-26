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

	scopeRoot = poolFactory.scope.alloc();
	scopeRoot->type = ScopeType::Module;
	scopeRoot->allocateSymTable();

	astRoot = Ast::newNode(&poolFactory.astNode, AstNodeType::Module, nullptr);
}

void Module::addFile(SourceFile* file)
{
    scoped_lock lk(mutexFile);
    file->module = this;
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
            file->module = nullptr;
            files.erase(it);
            return;
        }
    }

    assert(false);
}
