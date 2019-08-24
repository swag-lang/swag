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

void Module::setup(Workspace* wkp, const fs::path& pth)
{
    path   = pth;
    name   = path.filename().string();
    nameUp = name;
    makeUpper(nameUp);

    workspace = wkp;
    scopeRoot = Ast::newScope(nullptr, "", ScopeKind::Module, nullptr);
    scopeRoot->allocateSymTable();

    astRoot   = Ast::newNode(&g_Pool_astNode, AstNodeKind::Module, UINT32_MAX);
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
#ifdef _DEBUG
    for (auto r : availableRegistersRC)
        assert(r != reg);
#endif
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

        if (node->resultRegisterRC.size())
        {
            node->computedValue.reg = runContext->registersRC[node->resultRegisterRC[0]];
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

void Module::addDataSegmentInitString(uint32_t segOffset, uint32_t strIndex)
{
    scoped_lock lk(mutexString);
    strBufferDataSegInit[strIndex] = segOffset;
}

void Module::addConstantSegmentInitString(uint32_t segOffset, uint32_t strIndex)
{
    scoped_lock lk(mutexString);
    strBufferConstantSegInit[strIndex] = segOffset;
}

int Module::reserveDataSegment(int size)
{
    scoped_lock lk(mutexDataSeg);
    int         result = (int) dataSegment.size();
    dataSegment.resize((int) dataSegment.size() + size, 0);
    return result;
}

int Module::reserveConstantSegment(int size)
{
    scoped_lock lk(mutexConstantSeg);
    int         result = (int) constantSegment.size();
    constantSegment.resize((int) constantSegment.size() + size, 0);
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
