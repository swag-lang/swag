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
#include "BackendX64.h"
#include "ThreadManager.h"

bool Module::mustGenerateTestExe()
{
    if (!g_CommandLine.test)
        return false;
    if (!g_CommandLine.outputTest)
        return false;
    if (byteCodeTestFunc.empty())
        return false;
    if ((g_CommandLine.script))
        return false;
    if (g_Workspace.filteredModule && g_Workspace.filteredModule != this)
        return false;

    return true;
}

bool Module::canGenerateLegit()
{
    // Normal module
    if (!fromTestsFolder)
    {
        if (!g_CommandLine.outputLegit)
            return false;
    }

    // The test folder could generate normal modules (libraries) too
    else
    {
        if (!g_CommandLine.test)
            return false;
        if (!g_CommandLine.outputTest)
            return false;
        if (!byteCodeTestFunc.empty())
            return false;
        if (g_CommandLine.script)
            return false;
    }

    return true;
}

bool Module::setup(const Utf8& moduleName)
{
    unique_lock lk(mutexFile);
    if (setupDone)
        return true;
    setupDone = true;

    constantSegmentCompiler.compilerOnly = true;

    name   = moduleName;
    nameUp = name;
    nameUp.replaceAll('.', '_');
    nameDown = nameUp;
    nameUp.makeUpper();

    scopeRoot                      = Ast::newScope(nullptr, "", ScopeKind::Module, nullptr);
    astRoot                        = Ast::newNode<AstNode>(nullptr, AstNodeKind::Module, nullptr, nullptr);
    scopeRoot->owner               = astRoot;
    buildPass                      = g_CommandLine.buildPass;
    buildParameters.buildCfg       = &buildCfg;
    buildParameters.outputFileName = name.c_str();

    // Setup build configuration
    if (g_CommandLine.buildCfg == "debug")
    {
        buildCfg.byteCodeOptimize         = 1;
        buildCfg.safetyGuards             = true;
        buildCfg.backendOptimizeSpeed     = false;
        buildCfg.backendOptimizeSize      = false;
        buildCfg.backendDebugInformations = true;
    }
    else if (g_CommandLine.buildCfg == "fast-debug")
    {
        buildCfg.byteCodeOptimize         = 2;
        buildCfg.safetyGuards             = true;
        buildCfg.backendOptimizeSpeed     = true;
        buildCfg.backendOptimizeSize      = false;
        buildCfg.backendDebugInformations = true;
    }
    else if (g_CommandLine.buildCfg == "release")
    {
        buildCfg.byteCodeOptimize         = 2;
        buildCfg.safetyGuards             = false;
        buildCfg.backendOptimizeSpeed     = true;
        buildCfg.backendOptimizeSize      = false;
        buildCfg.backendDebugInformations = false;
    }

    // Overwrite with command line
    if (g_CommandLine.buildCfgOptimBC != "default")
        buildCfg.byteCodeOptimize = g_CommandLine.buildCfgOptimBC[0] - '0';
    if (g_CommandLine.buildCfgDebug != "default")
        buildCfg.backendDebugInformations = g_CommandLine.buildCfgDebug == "true" ? true : false;
    if (g_CommandLine.buildCfgOptimSpeed != "default")
        buildCfg.backendOptimizeSpeed = g_CommandLine.buildCfgOptimSpeed == "true" ? true : false;
    if (g_CommandLine.buildCfgOptimSize != "default")
        buildCfg.backendOptimizeSize = g_CommandLine.buildCfgOptimSize == "true" ? true : false;
    if (g_CommandLine.buildCfgSafety != "default")
        buildCfg.safetyGuards = g_CommandLine.buildCfgSafety == "true" ? true : false;

    return true;
}

void Module::allocateBackend()
{
    scoped_lock lk(mutexFile);
    if (backend)
        return;

    // Allocate backend, even if we do not want to output, because the backend can be used
    // to know if a build is necessary
    if (!hasUnittestError)
    {
        switch (g_CommandLine.backendType)
        {
        case BackendType::C:
            backend = new BackendC(this);
            break;
        case BackendType::LLVM:
            backend = new BackendLLVM(this);
            break;
        case BackendType::X64:
            backend = new BackendX64(this);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
    }
    else
    {
        // Dummy backend which does nothing, as unittest errors have no output
        backend = new Backend(this);
    }
}

void Module::addFile(SourceFile* file)
{
    scoped_lock lk(mutexFile);
    file->module        = this;
    file->indexInModule = (uint32_t) files.size();
    files.push_back(file);
    scopeRoot->addChildNoLock(file->scopePrivate);

    // Keep track of the most recent file
    moreRecentSourceFile = max(moreRecentSourceFile, file->writeTime);

    // If the file has nodes for the compiler pass, then we need to register it in the module
    {
        unique_lock lk1(file->mutexCompilerPass);
        if (!file->compilerPassFunctions.empty())
        {
            unique_lock lk2(mutexCompilerPass);
            filesForCompilerPass.insert(file);
        }
    }
}

void Module::removeFile(SourceFile* file)
{
    scoped_lock lk(mutexFile);
    scoped_lock lk1(scopeRoot->mutex);

    SWAG_ASSERT(file->module == this);

    auto idx = file->indexInModule;
    SWAG_ASSERT(files[idx] == file);
    files[idx]                = files.back();
    files[idx]->indexInModule = idx;
    files.pop_back();
    file->module        = nullptr;
    file->indexInModule = UINT32_MAX;
    scopeRoot->removeChildNoLock(file->scopePrivate);

    // If the file has compiler functions, then we need to unregister it from the module
    {
        unique_lock lk2(mutexCompilerPass);
        auto        it = filesForCompilerPass.find(file);
        if (it != filesForCompilerPass.end())
            filesForCompilerPass.erase(it);
    }
}

bool Module::executeNode(SourceFile* sourceFile, AstNode* node, JobContext* callerContext)
{
    // Only one run at a time !
    static mutex mutexExecuteNode;
    g_ThreadMgr.participate(mutexExecuteNode, AFFINITY_EXECBC);

    SWAG_ASSERT(node->flags & AST_BYTECODE_GENERATED);
    SWAG_ASSERT(node->flags & AST_BYTECODE_RESOLVED);
    SWAG_ASSERT(node->bc);
    SWAG_ASSERT(node->bc->out);

    bool result = executeNodeNoLock(sourceFile, node, callerContext);
    mutexExecuteNode.unlock();
    return result;
}

bool Module::executeNodeNoLock(SourceFile* sourceFile, AstNode* node, JobContext* callerContext)
{
#ifdef SWAG_HAS_ASSERT
    PushDiagnosticInfos di;
    if (g_CommandLine.devMode)
    {
        g_diagnosticInfos.last().sourceFile = sourceFile;
        g_diagnosticInfos.last().node       = node;
    }
#endif

    // Global setup
    runContext.callerContext = callerContext;
    runContext.setup(sourceFile, node, buildParameters.buildCfg->byteCodeStackSize);
    node->bc->enterByteCode(&runContext);

    auto module = sourceFile->module;
    bool result = module->runner.run(&runContext);
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

bool Module::sendCompilerMessage(CompilerMsgKind kind)
{
    ConcreteCompilerMessage msg;
    msg.kind = kind;
    return sendCompilerMessage(&msg);
}

bool Module::sendCompilerMessage(ConcreteCompilerMessage* msg)
{
    if (!canSendCompilerMessages)
        return true;

    unique_lock lk(mutexByteCodeCompiler);

    int index = (int) msg->kind;
    if (byteCodeCompiler[index].empty())
        return true;

    // Convert to a concrete message for the user
    msg->moduleName.buffer = (void*) name.c_str();
    msg->moduleName.count  = name.length();
    currentCompilerMessage = msg;

    for (auto bc : byteCodeCompiler[index])
    {
        SWAG_CHECK(executeNode(bc->node->sourceFile, bc->node, nullptr));
    }

    currentCompilerMessage = nullptr;

    return true;
}

bool Module::hasCompilerFuncFor(CompilerMsgKind kind)
{
    int index = (int) kind;
    return !byteCodeCompiler[index].empty();
}

void Module::addCompilerFunc(ByteCode* bc)
{
    auto funcDecl = CastAst<AstFuncDecl>(bc->node, AstNodeKind::FuncDecl);

    SWAG_ASSERT(funcDecl->parameters);
    SWAG_ASSERT(funcDecl->parameters->flags & AST_VALUE_COMPUTED);

    // Register the function in all the corresponding buckets
    auto filter = funcDecl->parameters->computedValue.reg.u64;
    for (uint32_t i = 0; i < 64; i++)
    {
        if (filter & ((uint64_t) 1 << i))
        {
            scoped_lock lk(mutexByteCodeCompiler);
            byteCodeCompiler[i].push_back(bc);
        }
    }
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
        auto attributeFlags = bc->node->attributeFlags;
        auto flags          = bc->node->flags;

        // Register for export
        if ((attributeFlags & ATTRIBUTE_PUBLIC) &&
            !(attributeFlags & ATTRIBUTE_INLINE) &&
            (!(flags & AST_FROM_GENERIC) || (flags & AST_FROM_BATCH)))
            bc->node->ownerScope->addPublicFunc(bc->node);

        if (attributeFlags & ATTRIBUTE_TEST_FUNC)
        {
            if (g_CommandLine.testFilter.empty() || strstr(bc->node->sourceFile->name, g_CommandLine.testFilter.c_str()))
                byteCodeTestFunc.push_back(bc);
        }
        else if (attributeFlags & ATTRIBUTE_INIT_FUNC)
        {
            byteCodeInitFunc.push_back(bc);
        }
        else if (attributeFlags & ATTRIBUTE_DROP_FUNC)
        {
            byteCodeDropFunc.push_back(bc);
        }
        else if (attributeFlags & ATTRIBUTE_RUN_FUNC)
        {
            byteCodeRunFunc.push_back(bc);
        }
        else if (attributeFlags & ATTRIBUTE_COMPILER_FUNC)
        {
            addCompilerFunc(bc);
        }
        else if (attributeFlags & ATTRIBUTE_MAIN_FUNC)
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
    for (auto& dep : moduleDependencies)
    {
        if (dep->name == importNode->name)
            return;
    }

    ModuleDependency* dep = g_Allocator.alloc<ModuleDependency>();
    dep->node             = importNode;
    dep->name             = importNode->name;
    moduleDependencies.push_front(dep);
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

uint32_t Module::mustOptimizeBC(AstNode* node)
{
    uint32_t level = buildCfg.byteCodeOptimize;
    if (node->attributeFlags & ATTRIBUTE_OPTIMIZEBC_0)
        level = 0;
    else if (node->attributeFlags & ATTRIBUTE_OPTIMIZEBC_1)
        level = 1;
    else if (node->attributeFlags & ATTRIBUTE_OPTIMIZEBC_2)
        level = 2;
    return level;
}

bool Module::hasBytecodeToRun()
{
    bool runByteCode = false;
    // If we have some #test functions, and we are in test mode
    if (g_CommandLine.test && g_CommandLine.runByteCodeTests && !byteCodeTestFunc.empty())
        runByteCode = true;
    // If we have #run functions
    else if (!byteCodeRunFunc.empty())
        runByteCode = true;
    // If we need to run in bytecode mode
    else if (g_CommandLine.run && g_CommandLine.script)
        runByteCode = true;
    return runByteCode;
}

bool Module::WaitForDependenciesDone(Job* job)
{
    for (auto& dep : moduleDependencies)
    {
        auto depModule = dep->module;
        SWAG_ASSERT(depModule);

        if (depModule->numErrors)
            continue;

        unique_lock lk(depModule->mutexDependency);
        if (depModule->hasBeenBuilt != BUILDRES_FULL)
        {
            depModule->dependentJobs.add(job);
            return false;
        }
    }

    return true;
}