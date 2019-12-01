#include "pch.h"
#include "Module.h"
#include "SourceFile.h"
#include "ByteCodeRun.h"
#include "Workspace.h"
#include "Scope.h"
#include "Ast.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "TypeManager.h"

Pool<Module> g_Pool_module;

void Module::setup(const string& moduleName)
{
    name   = moduleName;
    nameUp = name;
    replaceAll(nameUp, '.', '_');
    nameDown = nameUp;
    makeUpper(nameUp);

    scopeRoot              = Ast::newScope(nullptr, "", ScopeKind::Module, nullptr);
    astRoot                = Ast::newNode(nullptr, &g_Pool_astNode, AstNodeKind::Module, nullptr);
    scopeRoot->owner       = astRoot;
    buildPass              = g_CommandLine.buildPass;
    buildParameters.config = g_CommandLine.config;
    buildParameters.arch   = g_CommandLine.arch;
    buildParameters.target = g_Workspace.target;
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

    // Keep track of the most recent file
    auto   ftime = fs::last_write_time(file->path);
    time_t t     = fs::file_time_type::clock::to_time_t(ftime);
    if (files.size() == 1 || t > moreRecentSourceFile)
        moreRecentSourceFile = t;
}

void Module::removeFile(SourceFile* file)
{
    scoped_lock lk(mutexFile);
    scoped_lock lk1(scopeRoot->lockChilds);

    SWAG_ASSERT(file->module == this);

    auto idx = file->indexInModule;
    SWAG_ASSERT(files[idx] == file);
    files[idx]                = files.back();
    files[idx]->indexInModule = idx;
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
    static shared_mutex mutexExecuteNode;
    scoped_lock         lk(mutexExecuteNode);

    auto runContext = &g_Workspace.runContext;

    // Global setup
    {
        scoped_lock lkRR(mutexRegisterRR);
        runContext->setup(sourceFile, node, maxReservedRegisterRR, buildParameters.target.byteCodeStackSize);
        node->bc->enterByteCode(runContext);
    }

    if (!g_Run.run(&g_Workspace.runContext))
        return false;

    if (node->resultRegisterRC.size())
    {
        node->computedValue.reg = node->bc->registersRC[0][node->resultRegisterRC[0]];
        node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
        node->typeInfo = TypeManager::concreteType(node->typeInfo);
    }

    return true;
}

void Module::registerForeign(AstFuncDecl* node)
{
    scoped_lock lk(mutexByteCode);
    allForeign.push_back(node);
}

void Module::addByteCodeFunc(ByteCode* bc)
{
    SWAG_ASSERT(!bc->node || !(bc->node->attributeFlags & ATTRIBUTE_FOREIGN));

    scoped_lock lk(mutexByteCode);

    SWAG_ASSERT(!bc->addedToList);
    bc->addedToList = true;
    byteCodeFunc.push_back(bc);

    if (bc->node)
    {
        if (bc->node->attributeFlags & ATTRIBUTE_PUBLIC)
            bc->node->ownerScope->addPublicFunc(bc->node);

        if (bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC)
            byteCodeTestFunc.push_back(bc);
        else if (bc->node->attributeFlags & ATTRIBUTE_INIT_FUNC)
            byteCodeInitFunc.push_back(bc);
        else if (bc->node->attributeFlags & ATTRIBUTE_DROP_FUNC)
            byteCodeDropFunc.push_back(bc);
        else if (bc->node->attributeFlags & ATTRIBUTE_RUN_FUNC)
            byteCodeRunFunc.push_back(bc);
        else if (bc->node->attributeFlags & ATTRIBUTE_MAIN_FUNC)
        {
            SWAG_ASSERT(!byteCodeMainFunc);
            byteCodeMainFunc = bc;
        }
    }
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
    if (moduleDependencies.find(importNode->name) == moduleDependencies.end())
    {
        ModuleDependency dep;
        dep.node                             = importNode;
        dep.name                             = importNode->name;
        moduleDependencies[importNode->name] = dep;
    }
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

bool Module::internalError(const Utf8& msg)
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
    return false;
}

bool Module::internalError(uint32_t sourceFileIdx, SourceLocation& startLocation, SourceLocation& endLocation, const Utf8& msg)
{
    auto sourceFile = files[sourceFileIdx];
    return sourceFile->report({sourceFile, startLocation, endLocation, msg});
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