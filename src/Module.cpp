#include "pch.h"
#include "Module.h"
#include "Workspace.h"
#include "ByteCode.h"
#include "BackendLLVM.h"
#include "BackendX64.h"
#include "ThreadManager.h"
#include "Context.h"
#include "SemanticJob.h"
#include "ModuleManager.h"
#include "ErrorIds.h"
#include "CommandLine.h"
#include "LanguageSpec.h"

void Module::setup(const Utf8& moduleName, const Utf8& modulePath)
{
    ScopedLock lk(mutexFile);

    mutableSegment.setup(SegmentKind::Data, this);
    constantSegment.setup(SegmentKind::Constant, this);
    bssSegment.setup(SegmentKind::Bss, this);
    compilerSegment.setup(SegmentKind::Compiler, this);
    tlsSegment.setup(SegmentKind::Tls, this);

    handleGeneratedFile.set_size_clear(g_CommandLine->numCores);
    appendGeneratedFile.set_size_clear(g_CommandLine->numCores);
    countLinesGeneratedFile.set_size_clear(g_CommandLine->numCores);

    name           = moduleName;
    typeTable.name = moduleName;
    nameNormalized = name;
    nameNormalized.replaceAll('.', '_');
    path = modulePath.c_str();

    scopeRoot                      = Ast::newScope(nullptr, "", ScopeKind::Module, nullptr);
    astRoot                        = Ast::newNode<AstNode>(nullptr, AstNodeKind::Module, nullptr, nullptr);
    scopeRoot->owner               = astRoot;
    buildPass                      = g_CommandLine->buildPass;
    buildParameters.buildCfg       = &buildCfg;
    buildParameters.outputFileName = name;

    // Setup build configuration
    if (g_CommandLine->buildCfg == "fast-compile")
    {
        buildCfg.byteCodeOptimize         = false;
        buildCfg.byteCodeRemoveDup        = false;
        buildCfg.byteCodeDebugInline      = false;
        buildCfg.byteCodeInline           = false;
        buildCfg.byteCodeEmitAssume       = true;
        buildCfg.safetyGuards             = 0;
        buildCfg.stackTrace               = false;
        buildCfg.backendOptimizeSpeed     = false;
        buildCfg.backendOptimizeSize      = false;
        buildCfg.backendDebugInformations = false;
    }
    else if (g_CommandLine->buildCfg == "debug")
    {
        buildCfg.byteCodeOptimize         = false;
        buildCfg.byteCodeRemoveDup        = false;
        buildCfg.byteCodeDebugInline      = true;
        buildCfg.byteCodeInline           = true;
        buildCfg.byteCodeEmitAssume       = true;
        buildCfg.safetyGuards             = 0xFFFFFFFF'FFFFFFFF;
        buildCfg.stackTrace               = true;
        buildCfg.backendOptimizeSpeed     = false;
        buildCfg.backendOptimizeSize      = false;
        buildCfg.backendDebugInformations = true;
    }
    else if (g_CommandLine->buildCfg == "fast-debug")
    {
        buildCfg.byteCodeOptimize         = true;
        buildCfg.byteCodeRemoveDup        = false;
        buildCfg.byteCodeDebugInline      = false;
        buildCfg.byteCodeInline           = true;
        buildCfg.byteCodeEmitAssume       = true;
        buildCfg.safetyGuards             = 0xFFFFFFFF'FFFFFFFF;
        buildCfg.stackTrace               = true;
        buildCfg.backendOptimizeSpeed     = true;
        buildCfg.backendOptimizeSize      = false;
        buildCfg.backendDebugInformations = true;
    }
    else if (g_CommandLine->buildCfg == "release")
    {
        buildCfg.byteCodeOptimize         = true;
        buildCfg.byteCodeRemoveDup        = true;
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
    if (g_CommandLine->buildCfgInlineBC != "default")
        buildCfg.byteCodeInline = g_CommandLine->buildCfgInlineBC == "true" ? true : false;
    if (g_CommandLine->buildCfgOptimBC != "default")
        buildCfg.byteCodeOptimize = g_CommandLine->buildCfgOptimBC == "true" ? true : false;
    if (g_CommandLine->buildCfgRemoveDupBC != "default")
        buildCfg.byteCodeRemoveDup = g_CommandLine->buildCfgRemoveDupBC == "true" ? true : false;
    if (g_CommandLine->buildCfgDebug != "default")
        buildCfg.backendDebugInformations = g_CommandLine->buildCfgDebug == "true" ? true : false;
    if (g_CommandLine->buildCfgOptimSpeed != "default")
        buildCfg.backendOptimizeSpeed = g_CommandLine->buildCfgOptimSpeed == "true" ? true : false;
    if (g_CommandLine->buildCfgOptimSize != "default")
        buildCfg.backendOptimizeSize = g_CommandLine->buildCfgOptimSize == "true" ? true : false;
    if (g_CommandLine->buildCfgSafety != "default")
        buildCfg.safetyGuards = g_CommandLine->buildCfgSafety == "true" ? 0xFFFFFFFF'FFFFFFFF : 0;
    if (g_CommandLine->buildCfgStackTrace != "default")
        buildCfg.stackTrace = g_CommandLine->buildCfgStackTrace == "true" ? true : false;

    computePublicPath();
}

void Module::computePublicPath()
{
    if (kind == ModuleKind::BootStrap || kind == ModuleKind::Runtime)
        return;
    if (path.empty())
        return;

    publicPath = path + "/";
    publicPath += SWAG_PUBLIC_FOLDER;
    publicPath = Utf8::normalizePath(fs::path(publicPath.c_str()));

    if (!isScriptFile && kind != ModuleKind::Script && !isErrorModule)
    {
        if (!fs::exists(publicPath.c_str()))
        {
            error_code errorCode;
            if (!fs::create_directories(publicPath.c_str(), errorCode))
            {
                g_Log.errorOS(Fmt(Err(Err0543), publicPath.c_str()));
                OS::exit(-1);
            }
        }
    }

    publicPath += "/";
    publicPath.append(g_Workspace->getTargetFullName(g_CommandLine->buildCfg, g_CommandLine->target).c_str());

    if (!isScriptFile && kind != ModuleKind::Script && !isErrorModule)
    {
        if (!fs::exists(publicPath.c_str()))
        {
            error_code errorCode;
            if (!fs::create_directories(publicPath.c_str(), errorCode))
            {
                g_Log.errorOS(Fmt(Err(Err0543), publicPath.c_str()));
                OS::exit(-1);
            }
        }
    }

    publicPath += "/";
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
                reason = Fmt("forbidden character '%c'", name[i]);
                break;
            }
        }
    }

    if (error)
    {
        errorStr = Fmt("invalid module name `%s`, ", name.c_str());
        errorStr += reason;
        return false;
    }

    return true;
}

void Module::initFrom(Module* other)
{
    constantSegment.initFrom(&other->constantSegment);
    mutableSegment.initFrom(&other->mutableSegment);
    bssSegment.initFrom(&other->bssSegment);
    compilerSegment.initFrom(&other->compilerSegment);
    tlsSegment.initFrom(&other->tlsSegment);

    buildParameters.foreignLibs.insert(other->buildParameters.foreignLibs.begin(), other->buildParameters.foreignLibs.end());

    typeTable.initFrom(this, &other->typeTable);
}

void Module::buildModulesSlice()
{
    if (kind == ModuleKind::Config || kind == ModuleKind::BootStrap || kind == ModuleKind::Runtime)
        return;

    uint8_t* resultPtr;
    modulesSliceOffset = constantSegment.reserve((moduleDependencies.count + 1) * sizeof(SwagModule), &resultPtr);
    modulesSlice       = (SwagModule*) resultPtr;
    auto offset        = modulesSliceOffset;

    // Module name
    uint8_t* str;
    auto     offsetStr = constantSegment.addString(name, &str);
    constantSegment.addInitPtr(offset, offsetStr);
    *(uint8_t**) resultPtr = str;
    resultPtr += sizeof(void*);
    offset += sizeof(void*);
    *(uint64_t*) resultPtr = name.length();
    resultPtr += sizeof(void*);
    offset += sizeof(void*);

    // Slice of types
    *(uint8_t**) resultPtr = nullptr;
    resultPtr += sizeof(void*);
    offset += sizeof(void*);
    *(uint64_t*) resultPtr = 0;
    resultPtr += sizeof(void*);
    offset += sizeof(void*);

    for (auto& dep : moduleDependencies)
    {
        // Module name
        offsetStr = constantSegment.addString(dep->module->name, &str);
        constantSegment.addInitPtr(offset, offsetStr);
        *(uint8_t**) resultPtr = str;
        resultPtr += sizeof(void*);
        offset += sizeof(void*);
        *(uint64_t*) resultPtr = dep->module->name.length();
        resultPtr += sizeof(void*);
        offset += sizeof(void*);

        // Slice of types
        *(uint8_t**) resultPtr = nullptr;
        resultPtr += sizeof(void*);
        offset += sizeof(void*);
        *(uint64_t*) resultPtr = 0;
        resultPtr += sizeof(void*);
        offset += sizeof(void*);
    }

    SWAG_ASSERT((offset - modulesSliceOffset) == (moduleDependencies.count + 1) * sizeof(SwagModule));
}

void Module::buildTypesSlice()
{
    if (modulesSliceOffset == UINT32_MAX)
        return;

    auto&    map = typeTable.getMapPerSeg(&constantSegment).concreteTypes;
    uint8_t* resultPtr;
    uint32_t numTypes = (uint32_t) map.size();

    typesSliceOffset = constantSegment.reserve(sizeof(uint64_t) + (numTypes * sizeof(ConcreteTypeInfo*)), &resultPtr);
    auto offset      = typesSliceOffset;

    // First store the number of types in the table
    *(uint64_t*) resultPtr = numTypes;
    resultPtr += sizeof(uint64_t);
    offset += sizeof(uint64_t);

    // Initialize the "current module" slice of types
    auto moduleSlice          = (SwagModule*) constantSegment.address(modulesSliceOffset);
    moduleSlice->types.buffer = resultPtr;
    moduleSlice->types.count  = numTypes;
    constantSegment.addInitPtr(modulesSliceOffset + offsetof(SwagModule, types), typesSliceOffset + sizeof(uint64_t));

    for (auto t : map)
    {
        *(ConcreteTypeInfo**) resultPtr = t.second.concreteType;
        constantSegment.addInitPtr(offset, t.second.storageOffset);

        resultPtr += sizeof(ConcreteTypeInfo*);
        offset += sizeof(ConcreteTypeInfo*);
    }

    // Patch module list
    int i = 1;
    for (auto& dep : moduleDependencies)
    {
        auto callTable = dep->module->getGlobalPrivFct(g_LangSpec->name_getTypeTable);
        auto ptr       = g_ModuleMgr->getFnPointer(dep->module->name, callTable);
        if (!ptr)
        {
            moduleSlice[i].types.buffer = nullptr;
            moduleSlice[i].types.count  = 0;
        }
        else
        {
            typedef uint8_t* (*funcCall)();
            auto valPtr = ((funcCall) ptr)();

            moduleSlice[i].types.buffer = valPtr + sizeof(uint64_t);
            moduleSlice[i].types.count  = *(uint64_t*) valPtr;
        }

        i += 1;
    }
}

bool Module::canGenerateLegit()
{
    // Normal module
    if (kind != ModuleKind::Test)
    {
        if (!g_CommandLine->outputLegit)
            return false;
    }

    // The test folder could generate normal modules (libraries) too
    else
    {
        if (!g_CommandLine->test)
            return false;
        if (!g_CommandLine->outputTest)
            return false;
        if (!byteCodeTestFunc.empty())
            return false;
        if (g_CommandLine->scriptMode)
            return false;
    }

    return true;
}

SourceFile* Module::findFile(const Utf8& fileName)
{
    for (auto p : files)
    {
        if (Utf8::normalizePath(p->path) == Utf8::normalizePath(fileName.c_str()))
            return p;
    }

    return nullptr;
}

void Module::allocateBackend()
{
    ScopedLock lk(mutexFile);
    if (backend)
        return;

    // Allocate backend, even if we do not want to output, because the backend can be used
    // to know if a build is necessary
    if (!numTestErrors && !numTestWarnings && buildPass >= BuildPass::Backend && kind != ModuleKind::Runtime && kind != ModuleKind::BootStrap)
    {
        switch (g_CommandLine->backendGenType)
        {
        case BackendGenType::LLVM:
            backend = new BackendLLVM(this);
            break;
        case BackendGenType::X64:
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
    bssSegment.release();
    tlsSegment.release();

    for (auto& b : byteCodeFunc)
        b->releaseOut();
}

void Module::addExportSourceFile(SourceFile* file)
{
    ScopedLock lk(mutexFile);
    exportSourceFiles.insert(file);
}

void Module::addErrorModule(Module* module)
{
    ScopedLock lk(mutexFile);
    SWAG_ASSERT(!errorModules.contains(module));
    errorModules.push_back(module);
}

void Module::addFile(SourceFile* file)
{
    ScopedLock lk(mutexFile);
    ScopedLock lk1(scopeRoot->mutex);
    addFileNoLock(file);
}

void Module::addFileNoLock(SourceFile* file)
{
    file->module        = this;
    file->indexInModule = (uint32_t) files.size();
    files.push_back(file);

    // A file scope is not registered in the list of childs of
    // its parent
    if (file->scopeFile)
        file->scopeFile->parentScope = scopeRoot;

    // Keep track of the most recent file
    if (!file->writeTime)
        file->writeTime = OS::getFileWriteTime(file->path.c_str());
    moreRecentSourceFile = max(moreRecentSourceFile, file->writeTime);

    // If the file is flagged as '#global export', register it
    if (file->forceExport)
        exportSourceFiles.insert(file);
}

void Module::removeFile(SourceFile* file)
{
    ScopedLock lk(mutexFile);
    ScopedLock lk1(scopeRoot->mutex);

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

void Module::addGlobalVar(AstNode* node, GlobalVarKind varKind)
{
    ScopedLock lk(mutexGlobalVars);
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

void Module::addCompilerFunc(ByteCode* bc)
{
    auto funcDecl = CastAst<AstFuncDecl>(bc->node, AstNodeKind::FuncDecl);

    SWAG_ASSERT(funcDecl->parameters);
    SWAG_ASSERT(funcDecl->parameters->flags & AST_VALUE_COMPUTED);

    // Register the function in all the corresponding buckets
    auto filter = funcDecl->parameters->computedValue->reg.u64;
    for (uint32_t i = 0; i < 64; i++)
    {
        if (filter & ((uint64_t) 1 << i))
        {
            ScopedLock lk(byteCodeCompilerMutex[i]);
            SWAG_ASSERT(numCompilerFunctions > 0);
            numCompilerFunctions--;
            byteCodeCompiler[i].push_back(bc);
        }
    }
}

void Module::addByteCodeFunc(ByteCode* bc)
{
    ScopedLock lk(mutexByteCode);

    SWAG_ASSERT(!bc->node || !(bc->node->attributeFlags & ATTRIBUTE_FOREIGN));
    SWAG_ASSERT(!bc->isAddedToList);

    bc->isAddedToList = true;
    byteCodeFunc.push_back(bc);

    if (bc->node)
    {
        auto attributeFlags = bc->node->attributeFlags;
        auto flags          = bc->node->flags;

        // Register for export
        if ((attributeFlags & ATTRIBUTE_PUBLIC) &&
            !(attributeFlags & ATTRIBUTE_INLINE) &&
            !(attributeFlags & ATTRIBUTE_COMPILER) &&
            !(attributeFlags & ATTRIBUTE_INIT_FUNC) &&
            !(attributeFlags & ATTRIBUTE_DROP_FUNC) &&
            !(attributeFlags & ATTRIBUTE_PREMAIN_FUNC) &&
            !(attributeFlags & ATTRIBUTE_TEST_FUNC) &&
            !(flags & AST_FROM_GENERIC))
            bc->node->ownerScope->addPublicFunc(bc->node);

        if (attributeFlags & ATTRIBUTE_TEST_FUNC)
        {
            if (g_CommandLine->testFilter.empty() || strstr(bc->node->sourceFile->name, g_CommandLine->testFilter.c_str()))
                byteCodeTestFunc.push_back(bc);
        }
        else if (attributeFlags & ATTRIBUTE_INIT_FUNC)
        {
            byteCodeInitFunc.push_back(bc);
        }
        else if (attributeFlags & ATTRIBUTE_PREMAIN_FUNC)
        {
            byteCodePreMainFunc.push_back(bc);
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

void Module::addForeignLib(const Utf8& text)
{
    ScopedLock lk(mutexDependency);
    buildParameters.foreignLibs.insert(text);
}

bool Module::addInclude(AstNode* includeNode)
{
    ScopedLock lk(mutexDependency);
    includes.push_back(includeNode);
    return true;
}

bool Module::removeInclude(AstNode* includeNode)
{
    ScopedLock lk(mutexDependency);
    includes.erase_unordered_byval(includeNode);
    return true;
}

bool Module::addDependency(AstNode* importNode, const Token& tokenLocation, const Token& tokenVersion)
{
    ScopedLock lk(mutexDependency);
    for (auto& dep : moduleDependencies)
    {
        if (dep->name == importNode->token.text)
        {
            if (dep->location != tokenLocation.text && !tokenLocation.text.empty() && !dep->location.empty())
            {
                Diagnostic diag{importNode, tokenLocation, Fmt(Err(Err0284), dep->location.c_str())};
                Diagnostic note{dep->node, Nte(Nte0037), DiagnosticLevel::Note};
                return importNode->sourceFile->report(diag, &note);
            }

            if (dep->version != tokenVersion.text && !tokenVersion.text.empty() && !dep->version.empty())
            {
                Diagnostic diag{importNode, tokenVersion, Fmt(Err(Err0286), dep->version.c_str())};
                Diagnostic note{dep->node, Nte(Nte0037), DiagnosticLevel::Note};
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
    vector<Utf8> splits;
    Utf8::tokenize(dep->version.c_str(), '.', splits);

    if (splits.size() != 3 || splits[0].empty() || splits[1].empty() || splits[2].empty())
    {
        Diagnostic diag{importNode, tokenVersion, Err(Err0288)};
        Diagnostic note{dep->node, Err(Err0289), DiagnosticLevel::Note};
        return importNode->sourceFile->report(diag, &note);
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

        if (splits[i] == '?')
        {
            *setVer = -1;
            continue;
        }

        // Be sure we have a number
        for (int j = 0; j < splits[i].length(); j++)
        {
            if (!isdigit(splits[i][j]))
            {
                Diagnostic diag{importNode, tokenVersion, Err(Err0288)};
                Diagnostic note{dep->node, Err(Err0289), DiagnosticLevel::Note};
                return importNode->sourceFile->report(diag, &note);
            }
        }

        *setVer = atoi(splits[i]);
        if (*setVer < 0)
        {
            Diagnostic diag{importNode, tokenVersion, Err(Err0288)};
            Diagnostic note{dep->node, Err(Err0289), DiagnosticLevel::Note};
            return importNode->sourceFile->report(diag, &note);
        }

        switch (i)
        {
        case 1:
            SWAG_VERIFY(dep->verNum != -1, importNode->sourceFile->report({importNode, tokenVersion, Fmt(Err(Err0335), dep->revNum)}));
            break;
        case 2:
            SWAG_VERIFY(dep->revNum != -1, importNode->sourceFile->report({importNode, tokenVersion, Fmt(Err(Err0336), dep->buildNum)}));
            break;
        }
    }

    return true;
}

bool Module::removeDependency(AstNode* importNode)
{
    ScopedLock lk(mutexDependency);
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

bool Module::waitForDependenciesDone(Job* job, const set<Utf8>& modules)
{
    for (auto& dep : moduleDependencies)
    {
        auto depModule = dep->module;
        SWAG_ASSERT(depModule);

        if (depModule->numErrors)
            continue;
        if (modules.find(depModule->name) == modules.end())
            continue;

        ScopedLock lk(depModule->mutexDependency);
        if (depModule->hasBeenBuilt != BUILDRES_FULL)
        {
            job->waitingKind = JobWaitKind::DepDone;
            depModule->dependentJobs.add(job);
            return false;
        }

        g_ModuleMgr->loadModule(depModule->name);
    }

    return true;
}

bool Module::waitForDependenciesDone(Job* job)
{
    if (dependenciesDone)
        return true;

    for (auto& dep : moduleDependencies)
    {
        auto depModule = dep->module;
        SWAG_ASSERT(depModule);

        if (depModule->numErrors)
            continue;

        ScopedLock lk(depModule->mutexDependency);
        if (depModule->hasBeenBuilt != BUILDRES_FULL)
        {
            job->waitingKind = JobWaitKind::DepDone;
            depModule->dependentJobs.add(job);
            return false;
        }

        g_ModuleMgr->loadModule(depModule->name);
    }

    dependenciesDone = true;
    return true;
}

void Module::sortDependenciesByInitOrder(VectorNative<ModuleDependency*>& result)
{
    result = moduleDependencies;
    sort(result.begin(), result.end(), [](ModuleDependency* n1, ModuleDependency* n2)
         { return n2->module->hasDependencyTo(n1->module); });
}

void Module::setBuildPass(BuildPass buildP)
{
    ScopedLock lk(mutexBuildPass);
    buildPass = (BuildPass) min((int) buildP, (int) buildPass);
    buildPass = (BuildPass) min((int) g_CommandLine->buildPass, (int) buildPass);
}

void Module::setHasBeenBuilt(uint32_t buildResult)
{
    ScopedLock lk(mutexDependency);
    if (hasBeenBuilt == buildResult)
        return;
    hasBeenBuilt |= buildResult;
    dependentJobs.setRunning();
}

uint32_t Module::getHasBeenBuilt()
{
    SharedLock lk(mutexDependency);
    return hasBeenBuilt;
}

bool Module::error(const Utf8& msg)
{
    g_Log.lock();
    g_Log.setColor(LogColor::Red);
    g_Log.print("error: ");
    g_Log.print(Fmt("module %s: ", name.c_str()));
    g_Log.print(msg);
    g_Log.eol();
    g_Log.setDefaultColor();
    g_Log.unlock();

    g_Workspace->numErrors++;
    numErrors++;
    return false;
}

bool Module::internalError(const Utf8& msg)
{
    g_Log.lock();
    g_Log.setColor(LogColor::Red);
    g_Log.print("error: ");
    g_Log.print(Fmt("module %s: [compiler internal] ", name.c_str()));
    g_Log.print(msg);
    g_Log.eol();
    g_Log.setDefaultColor();
    g_Log.unlock();

    g_Workspace->numErrors++;
    numErrors++;
    return false;
}

bool Module::internalError(AstNode* node, Token& token, const Utf8& msg)
{
    Utf8 msg1 = "[compiler internal] ";
    msg1 += msg;
    return node->sourceFile->report({node, msg1});
}

void Module::printStartBuilding(const BuildParameters& bp)
{
    if (!backend->mustCompile)
    {
        g_Stats.skippedModules += 1;
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

void Module::printBC()
{
    for (auto bc : byteCodePrintBC)
        bc->print();
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
    if (g_CommandLine->test && g_CommandLine->runByteCodeTests && !byteCodeTestFunc.empty())
        runByteCode = true;
    // If we have #run functions
    else if (!byteCodeRunFunc.empty())
        runByteCode = true;
    // If we need to run in bytecode mode
    else if (g_CommandLine->run && g_CommandLine->scriptMode)
        runByteCode = true;
    return runByteCode;
}

bool Module::mustGenerateTestExe()
{
    if (!g_CommandLine->test)
        return false;
    if (!g_CommandLine->outputTest)
        return false;
    if (kind != ModuleKind::Test)
        return false;
    if (byteCodeTestFunc.empty())
        return false;
    if (g_CommandLine->scriptMode)
        return false;
    if (g_Workspace->filteredModule && g_Workspace->filteredModule != this)
        return false;
    if (numTestErrors || numTestWarnings)
        return false;

    return true;
}

bool Module::mustOutputSomething()
{
    bool mustOutput = true;
    // do not generate an executable that has been run in script mode
    if (byteCodeMainFunc && g_CommandLine->scriptMode)
        mustOutput = false;
    else if (kind == ModuleKind::BootStrap || kind == ModuleKind::Runtime)
        mustOutput = false;
    else if (buildPass < BuildPass::Backend)
        mustOutput = false;
    else if (files.empty())
        mustOutput = false;
    // a test module needs swag to be in test mode
    else if (kind == ModuleKind::Test && !g_CommandLine->outputTest)
        mustOutput = false;
    // if all files are exported, then do not generate a module
    else if (buildCfg.backendKind == BuildCfgBackendKind::Export)
        mustOutput = false;
    else if (kind != ModuleKind::Test && buildCfg.backendKind == BuildCfgBackendKind::None)
        mustOutput = false;

    return mustOutput;
}

bool Module::compileString(const Utf8& text)
{
    if (text.empty())
        return true;

    SWAG_ASSERT(g_RunContext.callerContext->baseJob);
    SWAG_ASSERT(g_RunContext.ip);
    SWAG_ASSERT(g_RunContext.ip->node);
    SWAG_ASSERT(g_RunContext.ip->node->sourceFile);

    auto sourceFile = g_RunContext.ip->node->sourceFile;

    // Is it still possible to generate some code ?
    if (!acceptsCompileString)
    {
        g_RunContext.ip->node->sourceFile->report({g_RunContext.ip->node, Err(Err0859)});
        return false;
    }

    AstNode*  parent = Ast::newNode(files[0], AstNodeKind::StatementNoScope, sourceFile->astRoot);
    SyntaxJob syntaxJob;
    syntaxJob.module = this;

    if (!syntaxJob.constructEmbedded(text, parent, g_RunContext.ip->node, CompilerAstKind::TopLevelInstruction, true))
        return false;

    auto job        = g_Allocator.alloc<SemanticJob>();
    job->sourceFile = files[0];
    job->module     = this;

    job->dependentJob = g_RunContext.callerContext->baseJob;
    while (job->dependentJob && job->dependentJob->dependentJob)
        job->dependentJob = job->dependentJob->dependentJob;

    job->nodes.push_back(parent);
    g_ThreadMgr.addJob(job);
    return true;
}

TypeInfoFuncAttr* Module::getRuntimeTypeFct(const Utf8& fctName)
{
    SharedLock lk(mutexFile);
    auto       it = mapRuntimeFcts.find(fctName);
    SWAG_ASSERT(it != mapRuntimeFcts.end());
    return it->second->typeInfoFunc;
}

ByteCode* Module::getRuntimeFct(const Utf8& fctName)
{
    SharedLock lk(mutexFile);
    auto       it = mapRuntimeFcts.find(fctName);
    SWAG_ASSERT(it != mapRuntimeFcts.end());
    return it->second;
}

void Module::addImplForToSolve(const Utf8& structName, uint32_t count)
{
    ScopedLock lk(mutexFile);

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
    ScopedLock lk(mutexFile);

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
    ScopedLock lk(mutexFile);

    auto it = implForToSolve.find(typeStruct->structName);
    SWAG_ASSERT(it != implForToSolve.end());
    SWAG_ASSERT(it->second.count != 0);
    it->second.count--;

    if (it->second.count == 0)
        it->second.dependentJobs.setRunning();
}

void Module::initProcessInfos()
{
    memcpy(&processInfos, &g_ProcessInfos, sizeof(SwagProcessInfos));
    processInfos.modules.buffer = modulesSlice;
    processInfos.modules.count  = moduleDependencies.count + 1;
}

void Module::callPreMain()
{
    for (auto& dep : moduleDependencies)
    {
        if (!dep->module->isSwag)
            continue;
        auto nameFct = dep->module->getGlobalPrivFct(g_LangSpec->name_globalPreMain);
        auto ptr     = g_ModuleMgr->getFnPointer(dep->name, nameFct);
        if (!ptr)
            continue;
        typedef void (*funcCall)(SwagProcessInfos*);
        ((funcCall) ptr)(&processInfos);
    }
}

Utf8 Module::getGlobalPrivFct(const Utf8& nameFct)
{
    return Fmt(nameFct.c_str(), nameNormalized.c_str());
}

void Module::logStage(const char* msg)
{
    if (!g_CommandLine->verboseStages)
        return;
    g_Log.verbose(Fmt("[%s] -- %s", name.c_str(), msg));
}

void Module::removeDuplicatedFunctions()
{
    unordered_map<uint32_t, ByteCode*> mapCrcBc;
    VectorNative<ByteCode*>            bufGenToTreat;

    bufGenToTreat.reserve((uint32_t) byteCodeFuncToGen.size());
    auto bufGen = move(byteCodeFuncToGen);

    // First pass : isolate functions that can be filtered and those that can't
    for (auto one : bufGen)
    {
        if (!one->crc || !one->typeInfoFunc || one->forceEmit)
        {
            byteCodeFuncToGen.push_back(one);
            continue;
        }

        if (one->node)
        {
            auto node = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);
            if (node->sourceFile->isBootstrapFile || node->sourceFile->isRuntimeFile)
            {
                byteCodeFuncToGen.push_back(one);
                continue;
            }
            if (node->attributeFlags & (ATTRIBUTE_PUBLIC | ATTRIBUTE_CALLBACK | ATTRIBUTE_MAIN_FUNC | ATTRIBUTE_INIT_FUNC | ATTRIBUTE_DROP_FUNC | ATTRIBUTE_PREMAIN_FUNC | ATTRIBUTE_TEST_FUNC))
            {
                byteCodeFuncToGen.push_back(one);
                continue;
            }
            if (node->specFlags & AST_SPEC_FUNCDECL_PATCH)
            {
                byteCodeFuncToGen.push_back(one);
                continue;
            }
        }

        // This is a valide candidate
        bufGenToTreat.push_back(one);
    }

    if (bufGenToTreat.empty())
        return;

    VectorNative<ByteCode*> stage2;
    while (true)
    {
        bool restart = false;
        stage2.clear();

        for (auto one : bufGenToTreat)
        {
            auto it = mapCrcBc.find(one->crc);
            if (it != mapCrcBc.end() && it->second != one)
            {
                bool toKick = true;

                // Test abi
                auto type0 = one->typeInfoFunc;
                auto type1 = it->second->typeInfoFunc;
                if (type0->parameters.size() != type1->parameters.size())
                    toKick = false;
                else if (type0->returnType && !type1->returnType)
                    toKick = false;
                else if (!type0->returnType && type1->returnType)
                    toKick = false;
                else if (type0->returnType->isNativeFloat() != type1->returnType->isNativeFloat())
                    toKick = false;
                else if (type0->returnType->sizeOf != type1->returnType->sizeOf)
                    toKick = false;
                else
                {
                    for (int i = 0; i < type0->parameters.size(); i++)
                    {
                        auto param0 = type0->parameters[i]->typeInfo;
                        auto param1 = type1->parameters[i]->typeInfo;
                        if (param0->isNativeFloat() != param1->isNativeFloat() || param0->sizeOf != param1->sizeOf)
                        {
                            toKick = false;
                            break;
                        }
                    }
                }

                // Test content
                if (toKick && !one->areSame(one->out, one->out + one->numInstructions, it->second->out, it->second->out + it->second->numInstructions, false, true))
                {
                    toKick = false;
                }

                if (toKick)
                {
                    numKickedFunc++;
                    one->alias = it->second;
                    restart    = true;
                    continue;
                }
            }
            else if (it == mapCrcBc.end())
            {
                mapCrcBc[one->crc] = one;
            }

            // If the function does not have local calls, then this is it, we are done with her
            // Otherwise, we put it for stage 2
            if (one->localCalls.empty())
                byteCodeFuncToGen.push_back(one);
            else
                stage2.push_back(one);
        }

        if (!restart || stage2.empty())
        {
            byteCodeFuncToGen.append(stage2);
            break;
        }

        // We recompute the crc of the functions, but only the part that are related to
        // local calls (by taking care of potential new aliases)
        for (auto f : stage2)
            f->computeCrcLocalCalls();

        bufGenToTreat = move(stage2);
    }
}

void Module::filterFunctionsToEmit()
{
    byteCodeFuncToGen.reserve((int) byteCodeFunc.size());
    for (auto bc : byteCodeFunc)
    {
        if (!bc->canEmit())
            continue;
        byteCodeFuncToGen.push_back(bc);
    }

    if (buildParameters.buildCfg->byteCodeOptimize && buildParameters.buildCfg->byteCodeRemoveDup)
        removeDuplicatedFunctions();
}