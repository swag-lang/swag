#include "pch.h"
#include "Module.h"
#include "ByteCodeRun.h"
#include "Workspace.h"
#include "Ast.h"
#include "ByteCode.h"
#include "ByteCodeStack.h"
#include "Diagnostic.h"
#include "TypeManager.h"
#include "BackendLLVM.h"
#include "BackendX64.h"
#include "ThreadManager.h"
#include "Context.h"
#include "SemanticJob.h"
#include "ModuleManager.h"

bool Module::mustGenerateTestExe()
{
    if (!g_CommandLine.test)
        return false;
    if (!g_CommandLine.outputTest)
        return false;
    if (!fromTestsFolder)
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
        buildCfg.byteCodeOptimize         = 2;
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

void Module::addPublicSourceFile(SourceFile* file)
{
    scoped_lock lk(mutexFile);
    publicSourceFiles.insert(file);
}

void Module::addFileNoLock(SourceFile* file)
{
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

    // If the file is flagged as #public, register it
    if (file->forcedPublic)
        publicSourceFiles.insert(file);
}

void Module::addFile(SourceFile* file)
{
    scoped_lock lk(mutexFile);
    scoped_lock lk1(scopeRoot->mutex);

    addFileNoLock(file);
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

    // If the file is flagged as #public, unregister it
    auto it = publicSourceFiles.find(file);
    if (it != publicSourceFiles.end())
        publicSourceFiles.erase(it);
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

    // Setup flags before running
    g_defaultContext.flags = getDefaultContextFlags(this);
    g_defaultContext.flags |= (uint64_t) ContextFlags::ByteCode;

    g_byteCodeStack.clear();
    bool result = executeNodeNoLock(sourceFile, node, callerContext);
    mutexExecuteNode.unlock();
    return result;
}

bool Module::executeNodeNoLock(SourceFile* sourceFile, AstNode* node, JobContext* callerContext)
{
    // Global setup
    runContext.callerContext = callerContext;
    runContext.setup(sourceFile, node, buildParameters.buildCfg->byteCodeStackSize);

    node->bc->enterByteCode(&runContext);
    auto module = sourceFile->module;
    bool result = module->runner.run(&runContext);
    node->bc->leaveByteCode(false);
    g_byteCodeStack.clear();

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

bool Module::sendCompilerMessage(CompilerMsgKind kind, Job* dependentJob)
{
    ConcreteCompilerMessage msg;
    msg.kind = kind;
    return sendCompilerMessage(&msg, dependentJob);
}

bool Module::sendCompilerMessage(ConcreteCompilerMessage* msg, Job* dependentJob)
{
    if (!runContext.canSendCompilerMessages)
        return true;

    unique_lock lk(mutexByteCodeCompiler);

    int index = (int) msg->kind;
    if (byteCodeCompiler[index].empty())
        return true;

    // Convert to a concrete message for the user
    msg->moduleName.buffer = (void*) name.c_str();
    msg->moduleName.count  = name.length();

    // Find to do that, as this function can only be called once (not multi threaded)
    runContext.currentCompilerMessage = msg;

    JobContext context;
    context.baseJob = dependentJob;

    for (auto bc : byteCodeCompiler[index])
    {
        SWAG_CHECK(executeNode(bc->node->sourceFile, bc->node, &context));
    }

    runContext.currentCompilerMessage = nullptr;

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

        // This is a real user function
        if (!(attributeFlags & ATTRIBUTE_FOREIGN) && !bc->node->sourceFile->isRuntimeFile)
            numConcreteBC++;

        // Register for export
        if ((attributeFlags & ATTRIBUTE_PUBLIC) && !(attributeFlags & ATTRIBUTE_INLINE) && !(flags & AST_FROM_GENERIC))
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

bool Module::error(const Utf8& msg)
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
    return false;
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
    Utf8 msg1 = "internal error: ";
    msg1 += msg;
    return node->sourceFile->report({node, node->token, msg1});
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

bool Module::mustOptimizeBC(AstNode* node)
{
    if (!node)
        return buildCfg.byteCodeOptimize != 0;
    if (node->attributeFlags & ATTRIBUTE_NO_OPTIM)
        return false;
    return true;
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
    if (dependenciesDone)
        return true;

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

        g_ModuleMgr.loadModule(depModule->name);
    }

    dependenciesDone = true;
    return true;
}

bool Module::mustOutputSomething()
{
    bool mustOutput = true;
    // do not generate an executable that have been run in script mode
    if (byteCodeMainFunc && g_CommandLine.script)
        mustOutput = false;
    // bootstrap
    else if (name.empty())
        mustOutput = false;
    else if (buildPass < BuildPass::Backend)
        mustOutput = false;
    else if (files.empty())
        mustOutput = false;
    // every files are published
    else if (files.size() == publicSourceFiles.size())
        mustOutput = false;
    // module must have unittest errors, so not output
    else if (hasUnittestError)
        mustOutput = false;
    else if (fromTestsFolder && !g_CommandLine.outputTest)
        mustOutput = false;
    // only foreign functions, or no function at all
    else if (numConcreteBC == 0)
        mustOutput = false;

    return mustOutput;
}

void Module::printBC()
{
    for (auto bc : byteCodePrintBC)
        bc->print();
}

bool Module::compileString(const Utf8& text)
{
    if (text.empty())
        return true;

    SWAG_ASSERT(runContext.callerContext->baseJob);
    SWAG_ASSERT(runContext.ip);
    SWAG_ASSERT(runContext.ip->node);
    SWAG_ASSERT(runContext.ip->node->sourceFile);

    auto sourceFile = runContext.ip->node->sourceFile;
    AstNode*  parent = Ast::newNode(files[0], AstNodeKind::StatementNoScope, sourceFile->astRoot);
    SyntaxJob syntaxJob;
    if (!syntaxJob.constructEmbedded(text, parent, runContext.ip->node, CompilerAstKind::TopLevelInstruction))
        return false;

    auto job          = g_Pool_semanticJob.alloc();
    job->sourceFile   = files[0];
    job->module       = this;
    job->dependentJob = runContext.callerContext->baseJob;
    job->nodes.push_back(parent);
    g_ThreadMgr.addJob(job);
    return true;
}