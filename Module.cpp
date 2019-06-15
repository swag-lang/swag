#include "pch.h"
#include "Module.h"
#include "SourceFile.h"
#include "ByteCodeRun.h"
#include "Workspace.h"
#include "Scope.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Log.h"
#include "ByteCode.h"
#include "Attribute.h"

Pool<Module> g_Pool_module;

void Module::setup(Workspace* wkp, const fs::path& pth, bool runtime)
{
    path   = pth;
    name   = path.filename().string();
    nameUp = name;
	makeUpper(nameUp);

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
    buildPass           = g_CommandLine.buildPass;
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

uint32_t Module::reserveRegisterRC(ByteCode* bc)
{
    scoped_lock lk(mutexRegisterRC);
    if (!availableRegistersRC.empty())
    {
        auto result = availableRegistersRC.back();
        bc->usedRegisters.insert(result);
        availableRegistersRC.pop_back();
        return result;
    }

    auto result = maxReservedRegisterRC++;
    bc->usedRegisters.insert(result);
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

        {
            scoped_lock lk1(mutexRegisterRC);
            scoped_lock lk2(mutexRegisterRR);
            runContext->setup(sourceFile, node, maxReservedRegisterRC, maxReservedRegisterRR, 1024);
        }

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

void Module::addByteCodeFunc(ByteCode* bc)
{
    scoped_lock lk(mutexByteCode);
    byteCodeFunc.push_back(bc);
    if (bc->node->attributeFlags & ATTRIBUTE_TEST)
        byteCodeTestFunc.push_back(bc);
}

void Module::setBuildPass(BuildPass buildP)
{
    scoped_lock lk(mutexBuildPass);
    buildPass = (BuildPass) min((int) buildP, (int) buildPass);
    buildPass = (BuildPass) min((int) g_CommandLine.buildPass, (int) buildPass);
}

void Module::error(const Utf8& msg)
{
    g_Log.lock();
    g_Log.setColor(LogColor::Red);
    g_Log.print(format("module %s: ", name.c_str()));
    g_Log.print("error: ");
    g_Log.print(msg);
    g_Log.eol();
    g_Log.setDefaultColor();
    g_Log.unlock();

    g_Workspace.numErrors++;
    numErrors++;
}

void Module::internalError(const Utf8& msg)
{
    g_Log.lock();
    g_Log.setColor(LogColor::Red);
    g_Log.print(format("module %s: ", name.c_str()));
    g_Log.print("internal error: ");
    g_Log.print(msg);
    g_Log.eol();
    g_Log.setDefaultColor();
    g_Log.unlock();

    g_Workspace.numErrors++;
    numErrors++;
}
