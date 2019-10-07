#include "pch.h"
#include "Module.h"
#include "SourceFile.h"
#include "ByteCodeRun.h"
#include "Workspace.h"
#include "Scope.h"
#include "Ast.h"
#include "Log.h"
#include "ByteCode.h"
#include "Attribute.h"
#include "Diagnostic.h"
#include "TypeManager.h"

Pool<Module> g_Pool_module;

void Module::setup(Workspace* wkp, const fs::path& pth)
{
    path   = pth;
    name   = path.filename().string();
    nameUp = name;
    makeUpper(nameUp);

    workspace = wkp;
    scopeRoot = Ast::newScope(nullptr, "", ScopeKind::Module, nullptr);
    scopeRoot->allocateSymTable();

    astRoot   = Ast::newNode(nullptr, &g_Pool_astNode, AstNodeKind::Module, UINT32_MAX);
    buildPass = g_CommandLine.buildPass;
}

void Module::addFile(SourceFile* file)
{
    scoped_lock lk(mutexFile);
    file->module        = this;
    file->indexInModule = (uint32_t) files.size();
    files.push_back(file);
    if (file->scopeRoot)
    {
        file->scopeRoot->indexInParent = (uint32_t) scopeRoot->childScopes.size();
        scopeRoot->childScopes.push_back(file->scopeRoot);
        file->scopeRoot->parentScope = scopeRoot;
    }
}

void Module::removeFile(SourceFile* file)
{
    scoped_lock lk(mutexFile);
    scoped_lock lk1(scopeRoot->lockChilds);

    SWAG_ASSERT(file->module == this);

    SWAG_ASSERT(files[file->indexInModule] == file);
    files[file->indexInModule]                = files.back();
    files[file->indexInModule]->indexInModule = file->indexInModule;
    files.pop_back();
    file->module        = nullptr;
    file->indexInModule = UINT32_MAX;

    auto indexInParent = file->scopeRoot->indexInParent;
    SWAG_ASSERT(scopeRoot->childScopes[indexInParent] == file->scopeRoot);
    scopeRoot->childScopes[indexInParent]                = scopeRoot->childScopes.back();
    scopeRoot->childScopes[indexInParent]->indexInParent = indexInParent;
    file->scopeRoot->indexInParent                       = UINT32_MAX;
}

void Module::reserveRegisterRR(uint32_t count)
{
    scoped_lock lk(mutexRegisterRR);
    maxReservedRegisterRR = max(maxReservedRegisterRR, count);
}

bool Module::executeNode(SourceFile* sourceFile, AstNode* node)
{
    // Only one run at a time !
    static SpinLock mutexExecuteNode;
    scoped_lock     lk(mutexExecuteNode);

    auto runContext = &workspace->runContext;

    // Global setup
    {
        scoped_lock lkRR(mutexRegisterRR);
        runContext->setup(sourceFile, node, maxReservedRegisterRR, g_CommandLine.byteCodeStackSize);
        node->bc->enterByteCode(runContext);
    }

    bool exception = false;
    if (!executeNodeNoLock(sourceFile, node, exception))
    {
        if (exception)
        {
            auto ip = runContext->ip - 1;
            sourceFile->report({runContext->bc->sourceFile, ip->startLocation, ip->endLocation, format("exception '%X' during bytecode execution !", m_exceptionCode)});
        }

        return false;
    }

    if (node->resultRegisterRC.size())
    {
        node->computedValue.reg = node->bc->registersRC[0][node->resultRegisterRC[0]];
        node->flags |= AST_VALUE_COMPUTED;
        node->typeInfo = TypeManager::concreteType(node->typeInfo);
    }

    return true;
}

bool Module::executeNodeNoLock(SourceFile* sourceFile, AstNode* node, bool& exception)
{
    __try
    {
        auto runContext = &workspace->runContext;
        if (!g_Run.run(runContext))
            return false;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        m_exceptionCode = GetExceptionCode();
        exception       = true;
        return false;
    }

    return true;
}

void Module::addByteCodeFunc(ByteCode* bc)
{
    scoped_lock lk(mutexByteCode);
    byteCodeFunc.push_back(bc);
    if (bc->node && bc->node->attributeFlags & ATTRIBUTE_TEST)
        byteCodeTestFunc.push_back(bc);
}

void Module::setBuildPass(BuildPass buildP)
{
    scoped_lock lk(mutexBuildPass);
    buildPass = (BuildPass) min((int) buildP, (int) buildPass);
    buildPass = (BuildPass) min((int) g_CommandLine.buildPass, (int) buildPass);
}

void Module::addDependency(AstNode* importNode)
{
    scoped_lock lk(mutexDependency);
    if (moduleDependenciesNames.find(importNode->name) == moduleDependenciesNames.end())
    {
        moduleDependenciesNames.insert(importNode->name);
        moduleDependencies.push_back(importNode);
    }
}

uint32_t Module::reserveString(const Utf8& str)
{
    scoped_lock lk(mutexString);
    auto        it = mapStrBuffer.find(str);
    if (it != mapStrBuffer.end())
        return it->second;

    strBuffer.push_back(str);
    uint32_t result   = (uint32_t)(strBuffer.size() - 1);
    mapStrBuffer[str] = result;
    return result;
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

void Module::deferReleaseChilds(AstNode* node)
{
    for (auto child : node->childs)
    {
        deferReleaseChilds(child);
        //child->release();
    }

    node->childs.clear();
}