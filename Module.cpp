#include "pch.h"
#include "Module.h"
#include "ByteCodeRun.h"
#include "Workspace.h"
#include "Ast.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "DiagnosticInfos.h"
#include "TypeManager.h"
#include "BackendC.h"
#include "BackendLLVM.h"
#include "ThreadManager.h"

bool Module::setup(const Utf8& moduleName)
{
    name   = moduleName;
    nameUp = name;
    nameUp.replaceAll('.', '_');
    nameDown = nameUp;
    nameUp.makeUpper();

    scopeRoot                      = Ast::newScope(nullptr, "", ScopeKind::Module, nullptr);
    astRoot                        = Ast::newNode<AstNode>(nullptr, AstNodeKind::Module, nullptr);
    scopeRoot->owner               = astRoot;
    buildPass                      = g_CommandLine.buildPass;
    buildParameters.buildCfg       = &buildCfg;
    buildParameters.outputFileName = name.c_str();

    // Setup build configuration
    if (g_CommandLine.buildCfg == "debug")
    {
        buildCfg.safetyGuards             = true;
        buildCfg.backendOptimizeSpeed     = false;
        buildCfg.backendOptimizeSize      = false;
        buildCfg.backendDebugInformations = true;
    }
    else if (g_CommandLine.buildCfg == "release")
    {
        buildCfg.safetyGuards             = true;
        buildCfg.backendOptimizeSpeed     = true;
        buildCfg.backendOptimizeSize      = false;
        buildCfg.backendDebugInformations = true;
    }
    else if (g_CommandLine.buildCfg == "final")
    {
        buildCfg.safetyGuards             = false;
        buildCfg.backendOptimizeSpeed     = true;
        buildCfg.backendOptimizeSize      = false;
        buildCfg.backendDebugInformations = false;
    }

    // Overwrite with command line
    if (g_CommandLine.buildCfgDebug != "default")
        buildCfg.backendDebugInformations = g_CommandLine.buildCfgDebug == "true" ? true : false;
    if (g_CommandLine.buildCfgOptimSpeed != "default")
        buildCfg.backendOptimizeSpeed = g_CommandLine.buildCfgOptimSpeed == "true" ? true : false;
    if (g_CommandLine.buildCfgOptimSize != "default")
        buildCfg.backendOptimizeSize = g_CommandLine.buildCfgOptimSize == "true" ? true : false;
    if (g_CommandLine.buildCfgSafety != "default")
        buildCfg.safetyGuards = g_CommandLine.buildCfgSafety == "true" ? true : false;

    // Allocate backend, even if we do not want to output, because the backend can be used
    // to know if a build is necessary
    switch (g_CommandLine.backendType)
    {
    case BackendType::Cl:
    case BackendType::Clang:
        backend = new BackendC(this);
        break;
    case BackendType::LLVM:
        backend = new BackendLLVM(this);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    return true;
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
    moreRecentSourceFile = max(moreRecentSourceFile, file->writeTime);
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
    static mutex mutexExecuteNode;
    g_ThreadMgr.participate(mutexExecuteNode, AFFINITY_EXECBC);
    bool result = executeNodeNoLock(sourceFile, node);
    mutexExecuteNode.unlock();
    return result;
}

bool Module::executeNodeNoLock(SourceFile* sourceFile, AstNode* node)
{
    auto runContext = &g_Workspace.runContext;

#ifdef SWAG_HAS_ASSERT
    PushDiagnosticInfos di;
    if (g_CommandLine.devMode)
    {
        g_diagnosticInfos.last().sourceFile = sourceFile;
        g_diagnosticInfos.last().node       = node;
    }
#endif

    // Global setup
    {
        scoped_lock lkRR(mutexRegisterRR);
        runContext->setup(sourceFile, node, maxReservedRegisterRR, buildParameters.buildCfg->byteCodeStackSize);
        node->bc->enterByteCode(runContext);
    }

    bool result = g_Run.run(&g_Workspace.runContext);
    node->bc->leaveByteCode();
    if (!result)
        return false;

    // Get result
    if (node->resultRegisterRC.size())
    {
        node->typeInfo = TypeManager::concreteType(node->typeInfo);
        if (node->typeInfo->isNative(NativeTypeKind::String))
        {
            SWAG_ASSERT(node->resultRegisterRC.size() == 2);
            const char* pz  = (const char*) node->bc->registersRC[0][node->resultRegisterRC[0]].pointer;
            uint32_t    len = node->bc->registersRC[0][node->resultRegisterRC[1]].u32;
            node->computedValue.text.reserve(len + 1);
            node->computedValue.text.count = len;
            memcpy(node->computedValue.text.buffer, pz, len);
            node->computedValue.text.buffer[len] = 0;
        }
        else
        {
            node->computedValue.reg = node->bc->registersRC[0][node->resultRegisterRC[0]];
        }

        node->setFlagsValueIsComputed();
    }

    // Free auto allocated memory
    for (auto ptr : node->bc->autoFree)
        free(ptr);

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
        if ((bc->node->attributeFlags & ATTRIBUTE_PUBLIC) && !(bc->node->attributeFlags & ATTRIBUTE_INLINE) && (!(bc->node->flags & AST_FROM_GENERIC)))
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

void Module::addForeignLib(const Utf8& text)
{
    scoped_lock lk(mutexDependency);
    buildParameters.foreignLibs.insert(text);
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

void Module::setHasBeenBuilt(uint32_t buildResult)
{
    unique_lock lk(mutexDependency);
    if (hasBeenBuilt == buildResult)
        return;
    hasBeenBuilt |= buildResult;
    dependentJobs.setRunning();
}

uint32_t Module::getHasBeenBuilt()
{
    shared_lock lk(mutexDependency);
    return hasBeenBuilt;
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

bool Module::internalError(AstNode* node, Token& token, const Utf8& msg)
{
    return node->sourceFile->report({node, node->token, msg});
}

void Module::printUserMessage(const BuildParameters& bp)
{
    if (!backend->mustCompile)
    {
        if (bp.compileType == BackendCompileType::Test)
            g_Log.messageHeaderCentered("Skipping build test", name.c_str(), LogColor::Gray);
        else
            g_Log.messageHeaderCentered("Skipping build", name.c_str(), LogColor::Gray);
    }
    else
    {
        const char* header = (bp.compileType == BackendCompileType::Test) ? "Building test" : "Building";
        g_Log.messageHeaderCentered(header, name.c_str());
    }
}

void Module::addGlobalVar(AstNode* node, bool bss)
{
    unique_lock lk(mutexGlobalVars);
    if (bss)
        globalVarsBss.push_back(node);
    else
        globalVarsMutable.push_back(node);
}

bool Module::mustEmitSafety(AstNode* node)
{
    bool safety = buildCfg.safetyGuards;
    if (node->attributeFlags & ATTRIBUTE_SAFETY_ON)
        safety = true;
    else if (node->attributeFlags & ATTRIBUTE_SAFETY_OFF)
        safety = false;
    return safety;
}
