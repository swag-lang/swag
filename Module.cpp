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
	AstScope::type = AstNodeType::RootModule;
	AstScope::name = "";
	AstScope::fullname = "";

    reset();
    allocateSymTable();
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

AstScope* Module::newNamespace(AstScope* parentNp, const utf8crc& nameNp)
{
	auto fullnameNp = parentNp->fullname + "." + nameNp;
	scoped_lock lk(mutexNamespace);
	auto it = namespaces.find(fullnameNp);
	if (it != namespaces.end())
		return it->second;
	auto np = poolFactory.astScope.alloc();
	namespaces[fullnameNp] = np;
	np->type = AstNodeType::Namespace;
	np->parentScope = this;
	np->name = nameNp;
	np->fullname = fullnameNp;
	return np;
}