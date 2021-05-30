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
#include "ErrorIds.h"

void Module::setup(const Utf8& moduleName, const Utf8& modulePath)
{
    unique_lock lk(mutexFile);
    if (setupDone)
        return;
    setupDone = true;

    compilerSegment.compilerOnly = true;
    mutableSegment.setup("mutable segment", this);
    constantSegment.setup("constant segment", this);
    bssSegment.setup("bss segment", this);
    typeSegment.setup("type segment", this);
    compilerSegment.setup("compiler segment", this);

    name   = moduleName;
    nameUp = name;
    nameUp.replaceAll('.', '_');
    nameDown = nameUp;
    nameUp.makeUpper();
    path = modulePath.c_str();

    scopeRoot                      = Ast::newScope(nullptr, "", ScopeKind::Module, nullptr);
    astRoot                        = Ast::newNode<AstNode>(nullptr, AstNodeKind::Module, nullptr, nullptr);
    scopeRoot->owner               = astRoot;
    buildPass                      = g_CommandLine.buildPass;
    buildParameters.buildCfg       = &buildCfg;
    buildParameters.outputFileName = name.c_str();

    // Setup build configuration
    if (g_CommandLine.buildCfg == "debug")
    {
        buildCfg.byteCodeOptimize         = false;
        buildCfg.byteCodeDebugInline      = true;
        buildCfg.byteCodeInline           = true;
        buildCfg.byteCodeEmitAssume       = true;
        buildCfg.safetyGuards             = 0xFFFFFFFF'FFFFFFFF;
        buildCfg.stackTrace               = true;
        buildCfg.backendOptimizeSpeed     = false;
        buildCfg.backendOptimizeSize      = false;
        buildCfg.backendDebugInformations = true;
    }
    else if (g_CommandLine.buildCfg == "fast-debug")
    {
        buildCfg.byteCodeOptimize         = true;
        buildCfg.byteCodeDebugInline      = false;
        buildCfg.byteCodeInline           = true;
        buildCfg.byteCodeEmitAssume       = true;
        buildCfg.safetyGuards             = 0xFFFFFFFF'FFFFFFFF;
        buildCfg.stackTrace               = true;
        buildCfg.backendOptimizeSpeed     = true;
        buildCfg.backendOptimizeSize      = false;
        buildCfg.backendDebugInformations = true;
    }
    else if (g_CommandLine.buildCfg == "release")
    {
        buildCfg.byteCodeOptimize         = true;
        buildCfg.byteCodeDebugInline      = false;
        buildCfg.byteCodeInline           = true;
        buildCfg.byteCodeEmitAssume       = false;
        buildCfg.safetyGuards             = 0;
        buildCfg.stackTrace               = false;
        buildCfg.backendOptimizeSpeed     = true;
        buildCfg.backendOptimizeSize      = false;
        buildCfg.backendDebugInformations = true;
    }

    // Overwrite with command line
    if (g_CommandLine.buildCfgInlineBC != "default")
        buildCfg.byteCodeInline = g_CommandLine.buildCfgInlineBC == "true" ? true : false;
    if (g_CommandLine.buildCfgOptimBC != "default")
        buildCfg.byteCodeOptimize = g_CommandLine.buildCfgOptimBC == "true" ? true : false;
    if (g_CommandLine.buildCfgDebug != "default")
        buildCfg.backendDebugInformations = g_CommandLine.buildCfgDebug == "true" ? true : false;
    if (g_CommandLine.buildCfgOptimSpeed != "default")
        buildCfg.backendOptimizeSpeed = g_CommandLine.buildCfgOptimSpeed == "true" ? true : false;
    if (g_CommandLine.buildCfgOptimSize != "default")
        buildCfg.backendOptimizeSize = g_CommandLine.buildCfgOptimSize == "true" ? true : false;
    if (g_CommandLine.buildCfgSafety != "default")
        buildCfg.safetyGuards = g_CommandLine.buildCfgSafety == "true" ? 0xFFFFFFFF'FFFFFFFF : 0;
    if (g_CommandLine.buildCfgStackTrace != "default")
        buildCfg.stackTrace = g_CommandLine.buildCfgStackTrace == "true" ? true : false;
}

bool Module::isValidName(const Utf8& name, Utf8& errorStr)
{
    Utf8 reason;
    bool error = false;

    if (name.length() == 0)
    {
        error  = true;
        reason = "name is empty";
    }
    else if (name.length() > 1 && name[0] == '_' && name[1] == '_')
    {
        error  = true;
        reason = "name cannot start with '__'";
    }
    else if (isdigit(name[0]))
    {
        error  = true;
        reason = "name cannot start with a digit number";
    }
    else
    {
        for (int i = 0; i < name.length(); i++)
        {
            if (name[i] <= 32)
            {
                error  = true;
                reason = "name cannot contain blank characters";
                break;
            }

            if (!isalnum(name[i]) && name[i] != '_')
            {
                error  = true;
                reason = format("forbidden character '%c'", name[i]);
                break;
            }
        }
    }

    if (error)
    {
        errorStr = format("invalid module name '%s', ", name.c_str());
        errorStr += reason;
        return false;
    }

    return true;
}

bool Module::mustGenerateTestExe()
{
    if (!g_CommandLine.test)
        return false;
    if (!g_CommandLine.outputTest)
        return false;
    if (kind != ModuleKind::Test)
        return false;
    if (byteCodeTestFunc.empty())
        return false;
    if (g_CommandLine.scriptMode)
        return false;
    if (g_Workspace.filteredModule && g_Workspace.filteredModule != this)
        return false;

    return true;
}

bool Module::canGenerateLegit()
{
    // Normal module
    if (kind != ModuleKind::Test)
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
        if (g_CommandLine.scriptMode)
            return false;
    }

    return true;
}

SourceFile* Module::findFile(const Utf8& fileName)
{
    for (auto p : files)
    {
        if (normalizePath(p->path) == normalizePath(fileName.c_str()))
            return p;
    }

    return nullptr;
}

void Module::allocateBackend()
{
    scoped_lock lk(mutexFile);
    if (backend)
        return;

    // Allocate backend, even if we do not want to output, because the backend can be used
    // to know if a build is necessary
    if (!numTestErrors && !numTestWarnings && buildPass >= BuildPass::Backend && kind != ModuleKind::Runtime && kind != ModuleKind::BootStrap)
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

void Module::release()
{
    constantSegment.release();
    compilerSegment.release();
    mutableSegment.release();
    typeSegment.release();
    bssSegment.release();
    tlsSegment.release();
}

void Module::addExportSourceFile(SourceFile* file)
{
    scoped_lock lk(mutexFile);
    exportSourceFiles.insert(file);
}

void Module::addFileNoLock(SourceFile* file)
{
    file->module        = this;
    file->indexInModule = (uint32_t) files.size();
    files.push_back(file);

    // A file private scope is not registers in the list of childs of
    // its parent
    if (file->scopePrivate)
        file->scopePrivate->parentScope = scopeRoot;

    // Keep track of the most recent file
    moreRecentSourceFile = max(moreRecentSourceFile, file->writeTime);

    // If the file is flagged as '#global export', register it
    if (file->forceExport)
        exportSourceFiles.insert(file);

    if (file->imported)
        importedSourceFiles.insert(file);
}

void Module::addErrorModule(Module* module)
{
    scoped_lock lk(mutexFile);
    errorModules.push_back(module);
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

    // If the file is flagged as '#global export', unregister it
    auto it1 = exportSourceFiles.find(file);
    if (it1 != exportSourceFiles.end())
        exportSourceFiles.erase(it1);
}

bool Module::executeNode(SourceFile* sourceFile, AstNode* node, JobContext* callerContext)
{
    // Only one run at a time !
    g_ThreadMgr.participate(mutexExecuteNode, AFFINITY_EXECBC);

    SWAG_ASSERT(node->semFlags & AST_SEM_BYTECODE_GENERATED);
    SWAG_ASSERT(node->semFlags & AST_SEM_BYTECODE_RESOLVED);
    SWAG_ASSERT(node->extension && node->extension->bc);
    SWAG_ASSERT(node->extension->bc->out);

    // Setup flags before running
    auto cxt = (SwagContext*) OS::tlsGetValue(g_tlsContextId);
    SWAG_ASSERT(cxt);
    cxt->flags = getDefaultContextFlags(this);
    cxt->flags |= (uint64_t) ContextFlags::ByteCode;

    g_byteCodeStack.clear();
    bool result = executeNodeNoLock(sourceFile, node, callerContext);
    mutexExecuteNode.unlock();
    return result;
}

bool Module::executeNodeNoLock(SourceFile* sourceFile, AstNode* node, JobContext* callerContext)
{
    // Global setup
    runContext.callerContext = callerContext;
    runContext.setup(sourceFile, node);

    node->extension->bc->enterByteCode(&runContext);
    auto module = sourceFile->module;

    // We need to take care of the room necessary in the stack, as bytecode instruction IncSPBP is not
    // generated for expression (just for functions)
    if (node->ownerScope)
    {
        runContext.decSP(node->ownerScope->startStackSize);

        // :opAffectConstExpr
        // Reserve room on the stack to store the result
        if (node->semFlags & AST_SEM_EXEC_RET_STACK)
            runContext.decSP(node->typeInfo->sizeOf);

        runContext.bp = runContext.sp;
    }

    bool result = module->runner.run(&runContext);

    node->extension->bc->leaveByteCode(&runContext, false);
    g_byteCodeStack.clear();

    if (!result)
        return false;

    // :opAffectConstExpr
    // Result is on the stack. Store it in the compiler segment.
    if (node->semFlags & AST_SEM_EXEC_RET_STACK)
    {
        auto offsetStorage                = sourceFile->module->compilerSegment.reserve(node->typeInfo->sizeOf);
        node->computedValue.storageOffset = offsetStorage;
        auto addrDst                      = sourceFile->module->compilerSegment.address(offsetStorage);
        auto addrSrc                      = runContext.bp;
        memcpy(addrDst, addrSrc, node->typeInfo->sizeOf);
    }

    // Transform result to a literal value
    else if (node->resultRegisterRC.size())
    {
        node->typeInfo = TypeManager::concreteReferenceType(node->typeInfo);
        if (node->typeInfo->isNative(NativeTypeKind::String))
        {
            SWAG_ASSERT(node->resultRegisterRC.size() == 2);
            const char* pz  = (const char*) runContext.registersRC[0]->buffer[node->resultRegisterRC[0]].pointer;
            uint32_t    len = runContext.registersRC[0]->buffer[node->resultRegisterRC[1]].u32;
            node->computedValue.text.reserve(len + 1);
            node->computedValue.text.count = len;
            memcpy(node->computedValue.text.buffer, pz, len);
            node->computedValue.text.buffer[len] = 0;
        }
        else if (node->typeInfo->flags & TYPEINFO_RETURN_BY_COPY)
        {
            bool ok = false;
            if (node->typeInfo->kind == TypeInfoKind::Struct && node->typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
                ok = true;
            if (node->typeInfo->kind == TypeInfoKind::Struct && node->typeInfo->declNode->attributeFlags & ATTRIBUTE_CONSTEXPR)
                ok = true;
            if (node->typeInfo->kind == TypeInfoKind::Array)
                ok = true;

            if (!ok)
            {
                if (node->typeInfo->kind == TypeInfoKind::Struct && !(node->typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE))
                    return callerContext->report({node, format(Msg0281, node->typeInfo->getDisplayName().c_str())});
                return callerContext->report({node, format(Msg0280, node->typeInfo->getDisplayName().c_str())});
            }

            auto offsetStorage                = sourceFile->module->constantSegment.reserve(node->typeInfo->sizeOf);
            node->computedValue.storageOffset = offsetStorage;
            auto addrDst                      = sourceFile->module->constantSegment.address(offsetStorage);
            auto addrSrc                      = runContext.registersRR[0].pointer;
            memcpy(addrDst, (const void*) addrSrc, node->typeInfo->sizeOf);
        }
        else
        {
            switch (node->typeInfo->sizeOf)
            {
            case 1:
                node->computedValue.reg.u64 = runContext.registersRC[0]->buffer[node->resultRegisterRC[0]].u8;
                break;
            case 2:
                node->computedValue.reg.u64 = runContext.registersRC[0]->buffer[node->resultRegisterRC[0]].u16;
                break;
            case 4:
                node->computedValue.reg.u64 = runContext.registersRC[0]->buffer[node->resultRegisterRC[0]].u32;
                break;
            default:
                node->computedValue.reg.u64 = runContext.registersRC[0]->buffer[node->resultRegisterRC[0]].u64;
                break;
            }
        }

        node->setFlagsValueIsComputed();
    }

    // Free auto allocated memory
    for (auto ptr : node->extension->bc->autoFree)
        g_Allocator.free(ptr.first, ptr.second);

    return true;
}

void Module::postCompilerMessage(ConcreteCompilerMessage& msg)
{
    unique_lock lk(mutexByteCodeCompiler);

    // Cannot decide yet if there's a corresponding #compiler for that message, so push it
    if (numCompilerFunctions > 0)
        compilerMessages.push_back(msg);

    // We can decide, because every #compiler function have been registered.
    // So if there's no #compiler function for the given message, just dismiss it.
    else
    {
        int index = (int) msg.kind;
        if (byteCodeCompiler[index].empty())
            return;
        compilerMessages.push_back(msg);
    }
}

bool Module::flushCompilerMessages(JobContext* context)
{
    while (!compilerMessages.empty())
    {
        auto& msg = compilerMessages.back();

        // Be sure we have a #compiler for that message
        int index = (int) msg.kind;
        if (byteCodeCompiler[index].empty())
        {
            compilerMessages.pop_back();
            continue;
        }

        if (msg.kind == CompilerMsgKind::SemanticFunc)
        {
            uint32_t storageOffset;
            context->sourceFile = files.front();
            SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, (TypeInfo*) msg.type, nullptr, &storageOffset, CONCRETE_SHOULD_WAIT | CONCRETE_FOR_COMPILER));
            if (context->result != ContextResult::Done)
                return true;
            msg.type = (ConcreteTypeInfo*) compilerSegment.address(storageOffset);
        }

        sendCompilerMessage(&msg, context->baseJob);
        SWAG_ASSERT(context->result == ContextResult::Done);

        compilerMessages.pop_back();
    }

    return true;
}

bool Module::sendCompilerMessage(CompilerMsgKind msgKind, Job* dependentJob)
{
    ConcreteCompilerMessage msg;
    msg.kind = msgKind;
    return sendCompilerMessage(&msg, dependentJob);
}

bool Module::sendCompilerMessage(ConcreteCompilerMessage* msg, Job* dependentJob)
{
    unique_lock lk(mutexByteCodeCompiler);
    int         index = (int) msg->kind;
    if (byteCodeCompiler[index].empty())
        return true;

    // Convert to a concrete message for the user
    msg->moduleName.buffer = (void*) name.c_str();
    msg->moduleName.count  = name.length();

    // Find to do that, as this function can only be called once (not multi threaded)
    runContext.currentCompilerMessage = msg;

    JobContext context;
    context.baseJob = dependentJob;

    PushSwagContext cxt;
    for (auto bc : byteCodeCompiler[index])
    {
        SWAG_CHECK(executeNode(bc->node->sourceFile, bc->node, &context));
    }

    runContext.currentCompilerMessage = nullptr;
    return true;
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
            SWAG_ASSERT(numCompilerFunctions > 0);
            numCompilerFunctions--;
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
            !(attributeFlags & ATTRIBUTE_COMPILER) &&
            !(flags & AST_FROM_GENERIC))
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

bool Module::removeDependency(AstNode* importNode)
{
    scoped_lock lk(mutexDependency);
    for (int i = 0; i < moduleDependencies.size(); i++)
    {
        if (moduleDependencies[i]->node == importNode)
        {
            moduleDependencies.erase(i);
            return true;
        }
    }

    return true;
}

bool Module::addDependency(AstNode* importNode, const Token& tokenLocation, const Token& tokenVersion)
{
    scoped_lock lk(mutexDependency);
    for (auto& dep : moduleDependencies)
    {
        if (dep->name == importNode->token.text)
        {
            if (dep->location != tokenLocation.text && !tokenLocation.text.empty() && !dep->location.empty())
            {
                Diagnostic diag{importNode, tokenLocation, format(Msg0284, dep->location.c_str())};
                Diagnostic note{dep->node, Msg0287, DiagnosticLevel::Note};
                return importNode->sourceFile->report(diag, &note);
            }

            if (dep->version != tokenVersion.text && !tokenVersion.text.empty() && !dep->version.empty())
            {
                Diagnostic diag{importNode, tokenVersion, format(Msg0286, dep->version.c_str())};
                Diagnostic note{dep->node, Msg0287, DiagnosticLevel::Note};
                return importNode->sourceFile->report(diag, &note);
            }

            return true;
        }
    }

    ModuleDependency* dep = g_Allocator.alloc<ModuleDependency>();
    dep->node             = importNode;
    dep->name             = importNode->token.text;
    dep->location         = tokenLocation.text;
    dep->version          = tokenVersion.text.empty() ? "?.?.?" : tokenVersion.text;
    dep->tokenLocation    = tokenLocation;
    dep->tokenVersion     = tokenVersion;
    moduleDependencies.push_front(dep);

    // Check version
    bool         invalidVersion = false;
    vector<Utf8> splits;
    tokenize(dep->version.c_str(), '.', splits);

    while (true)
    {
        if (splits.size() != 3 || splits[0].empty() || splits[1].empty() || splits[2].empty())
        {
            invalidVersion = true;
            break;
        }

        int* setVer = nullptr;
        for (int i = 0; i < 3; i++)
        {
            switch (i)
            {
            case 0:
                setVer = &dep->verNum;
                break;
            case 1:
                setVer = &dep->revNum;
                break;
            case 2:
                setVer = &dep->buildNum;
                break;
            }

            if (splits[i] == "?")
            {
                *setVer = -1;
                continue;
            }

            for (int j = 0; j < splits[i].length(); j++)
            {
                if (!isdigit(splits[i][j]))
                    invalidVersion = true;
            }

            *setVer = atoi(splits[i]);
            if (*setVer < 0)
                invalidVersion = true;
        }

        break;
    }

    if (invalidVersion)
    {
        Diagnostic diag{importNode, tokenVersion, Msg0288};
        Diagnostic note{dep->node, Msg0289, DiagnosticLevel::Note};
        return importNode->sourceFile->report(diag, &note);
        return false;
    }

    return true;
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
    g_Log.print("error: ");
    g_Log.print(format("module %s: ", name.c_str()));
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
    g_Log.print("error: ");
    g_Log.print(format("module %s: [compiler internal] ", name.c_str()));
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
    Utf8 msg1 = "[compiler internal] ";
    msg1 += msg;
    return node->sourceFile->report({node, node->token, msg1});
}

void Module::printUserMessage(const BuildParameters& bp)
{
    if (!backend->mustCompile)
    {
        g_Stats.skippedModules += 1;
        if (g_CommandLine.verbosePass)
        {
            if (bp.compileType == BackendCompileType::Test)
                g_Log.messageHeaderCentered("Skipping build test", name.c_str(), LogColor::Gray);
            else if (bp.compileType == BackendCompileType::Example)
                g_Log.messageHeaderCentered("Skipping build example", name.c_str(), LogColor::Gray);
            else
                g_Log.messageHeaderCentered("Skipping build", name.c_str(), LogColor::Gray);
        }
    }
    else
    {
        if (bp.compileType == BackendCompileType::Test)
            g_Log.messageHeaderCentered("Building test", name.c_str());
        else if (bp.compileType == BackendCompileType::Example)
            g_Log.messageHeaderCentered("Building example", name.c_str());
        else
            g_Log.messageHeaderCentered("Building", name.c_str());
    }
}

void Module::addGlobalVar(AstNode* node, GlobalVarKind varKind)
{
    unique_lock lk(mutexGlobalVars);
    switch (varKind)
    {
    case GlobalVarKind::Mutable:
        globalVarsMutable.push_back(node);
        break;
    case GlobalVarKind::Bss:
        globalVarsBss.push_back(node);
        break;
    case GlobalVarKind::Constant:
        globalVarsConstant.push_back(node);
        break;
    }
}

bool Module::mustEmitSafetyOF(AstNode* node)
{
    return mustEmitSafety(node, ATTRIBUTE_SAFETY_OVERFLOW_ON, ATTRIBUTE_SAFETY_OVERFLOW_OFF);
}

bool Module::mustEmitSafety(AstNode* node, uint64_t whatOn, uint64_t whatOff)
{
    // Special operator version without overflow checking
    if (whatOff == ATTRIBUTE_SAFETY_OVERFLOW_OFF && node->attributeFlags & ATTRIBUTE_SAFETY_OFF_OPERATOR)
        return false;

    return ((buildCfg.safetyGuards & whatOn) || (node->attributeFlags & whatOn)) && !(node->attributeFlags & whatOff);
}

bool Module::mustOptimizeBC(AstNode* node)
{
    if (!node)
        return buildCfg.byteCodeOptimize;

    while (node)
    {
        if (node->attributeFlags & ATTRIBUTE_OPTIM_BYTECODE_OFF)
            return false;
        if (node->attributeFlags & ATTRIBUTE_OPTIM_BYTECODE_ON)
            return true;
        node = node->ownerFct;
    }

    return buildCfg.byteCodeOptimize;
}

bool Module::mustOptimizeBK(AstNode* node)
{
    if (!node)
        return buildCfg.byteCodeOptimize;
    return (buildCfg.byteCodeOptimize || (node->attributeFlags & ATTRIBUTE_OPTIM_BACKEND_ON)) && !(node->attributeFlags & ATTRIBUTE_OPTIM_BACKEND_OFF);
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
    else if (g_CommandLine.run && g_CommandLine.scriptMode)
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

bool Module::areAllFilesExported()
{
    return buildCfg.backendKind == BuildCfgBackendKind::Export;
}

bool Module::mustOutputSomething()
{
    bool mustOutput = true;
    // do not generate an executable that has been run in script mode
    if (byteCodeMainFunc && g_CommandLine.scriptMode)
        mustOutput = false;
    // bootstrap
    else if (name.empty())
        mustOutput = false;
    else if (buildPass < BuildPass::Backend)
        mustOutput = false;
    else if (files.empty())
        mustOutput = false;
    // module must have unittest errors, so no output
    else if (numTestErrors)
        mustOutput = false;
    else if (numTestWarnings)
        mustOutput = false;
    // a test module needs swag to be in test mode
    else if (kind == ModuleKind::Test && !g_CommandLine.outputTest)
        mustOutput = false;
    // if all files are exported, then do not generate a module
    else if (areAllFilesExported())
        mustOutput = false;
    else if (kind != ModuleKind::Test && buildCfg.backendKind == BuildCfgBackendKind::None)
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

    auto      sourceFile = runContext.ip->node->sourceFile;
    AstNode*  parent     = Ast::newNode(files[0], AstNodeKind::StatementNoScope, sourceFile->astRoot);
    SyntaxJob syntaxJob;
    if (!syntaxJob.constructEmbedded(text, parent, runContext.ip->node, CompilerAstKind::TopLevelInstruction, true))
        return false;

    auto job          = g_Pool_semanticJob.alloc();
    job->sourceFile   = files[0];
    job->module       = this;
    job->dependentJob = runContext.callerContext->baseJob;
    job->nodes.push_back(parent);
    g_ThreadMgr.addJob(job);
    return true;
}

bool Module::hasDependencyTo(Module* module)
{
    for (auto dep : moduleDependencies)
    {
        if (dep->module == module)
            return true;
        if (dep->module->hasDependencyTo(module))
            return true;
    }

    return false;
}

void Module::sortDependenciesByInitOrder(VectorNative<ModuleDependency*>& result)
{
    result = moduleDependencies;
    sort(result.begin(), result.end(), [](ModuleDependency* n1, ModuleDependency* n2) {
        return n2->module->hasDependencyTo(n1->module);
    });
}

TypeInfoFuncAttr* Module::getRuntimeTypeFct(const char* fctName)
{
    unique_lock lk(mutexFile);
    auto        it = mapRuntimeFcts.find(fctName);
    SWAG_ASSERT(it != mapRuntimeFcts.end());
    return it->second->typeInfoFunc;
}

ByteCode* Module::getRuntimeFct(const char* fctName)
{
    unique_lock lk(mutexFile);
    auto        it = mapRuntimeFcts.find(fctName);
    SWAG_ASSERT(it != mapRuntimeFcts.end());
    return it->second;
}

void Module::addImplForToSolve(const Utf8& structName, uint32_t count)
{
    unique_lock lk(mutexFile);

    auto it = implForToSolve.find(structName);
    if (it == implForToSolve.end())
    {
        implForToSolve[structName] = {};
        it                         = implForToSolve.find(structName);
    }

    it->second.count += count;
}

bool Module::waitImplForToSolve(Job* job, TypeInfoStruct* typeStruct)
{
    unique_lock lk(mutexFile);

    if (typeStruct->declNode && typeStruct->declNode->flags & AST_FROM_GENERIC)
        return false;
    auto it = implForToSolve.find(typeStruct->structName);
    if (it == implForToSolve.end())
        return false;
    if (it->second.count == 0)
        return false;

    it->second.dependentJobs.add(job);
    return true;
}

void Module::decImplForToSolve(TypeInfoStruct* typeStruct)
{
    unique_lock lk(mutexFile);

    auto it = implForToSolve.find(typeStruct->structName);
    SWAG_ASSERT(it != implForToSolve.end());
    SWAG_ASSERT(it->second.count != 0);
    it->second.count--;

    if (it->second.count == 0)
        it->second.dependentJobs.setRunning();
}