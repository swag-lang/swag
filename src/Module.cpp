#include "pch.h"
#include "Module.h"
#include "LLVM.h"
#include "SCBE.h"
#include "ByteCode.h"
#include "Context.h"
#include "LanguageSpec.h"
#include "ModuleManager.h"
#include "Parser.h"
#include "Report.h"
#include "SaveGenJob.h"
#include "SemanticError.h"
#include "SemanticJob.h"
#include "ThreadManager.h"
#include "Workspace.h"

void Module::setup(const Utf8& moduleName, const Path& modulePath)
{
    ScopedLock lk(mutexFile);

    buildParameters.module = this;
    mutableSegment.setup(SegmentKind::Data, this);
    constantSegment.setup(SegmentKind::Constant, this);
    bssSegment.setup(SegmentKind::Bss, this);
    compilerSegment.setup(SegmentKind::Compiler, this);
    tlsSegment.setup(SegmentKind::Tls, this);

    contentJobGeneratedFile.resize(g_ThreadMgr.numWorkers);
    countLinesGeneratedFile.set_size_clear(g_ThreadMgr.numWorkers);

    compilerSegmentPerThread.set_size_clear(g_ThreadMgr.numWorkers);
    for (uint32_t i = 0; i < g_ThreadMgr.numWorkers; i++)
    {
        compilerSegmentPerThread[i] = new DataSegment;
        compilerSegmentPerThread[i]->setup(SegmentKind::Compiler, this);
        compilerSegmentPerThread[i]->compilerThreadIdx = i;
    }

    name = moduleName;
    typeGen.setup(moduleName);
    nameNormalized = name;
    nameNormalized.replaceAll('.', '_');
    path = modulePath;

    scopeRoot                      = Ast::newScope(nullptr, "", ScopeKind::Module, nullptr);
    astRoot                        = Ast::newNode<AstNode>(nullptr, AstNodeKind::Module, nullptr, nullptr);
    scopeRoot->owner               = astRoot;
    buildPass                      = g_CommandLine.buildPass;
    buildParameters.buildCfg       = &buildCfg;
    buildParameters.outputFileName = name;

    // Setup build configuration
    if (g_CommandLine.buildCfg == "fast-compile")
    {
        buildCfg.byteCodeOptimizeLevel    = 0;
        buildCfg.byteCodeInline           = false;
        buildCfg.byteCodeAutoInline       = false;
        buildCfg.byteCodeEmitAssume       = true;
        buildCfg.safetyGuards             = 0;
        buildCfg.errorStackTrace          = false;
        buildCfg.debugAllocator           = true;
        buildCfg.backendOptimize          = BuildCfgBackendOptim::O0;
        buildCfg.backendDebugInformations = false;
    }
    else if (g_CommandLine.buildCfg == "debug")
    {
        buildCfg.byteCodeOptimizeLevel    = 0;
        buildCfg.byteCodeInline           = false;
        buildCfg.byteCodeAutoInline       = false;
        buildCfg.byteCodeEmitAssume       = true;
        buildCfg.safetyGuards             = SAFETY_ALL;
        buildCfg.errorStackTrace          = true;
        buildCfg.debugAllocator           = true;
        buildCfg.backendOptimize          = BuildCfgBackendOptim::O0;
        buildCfg.backendDebugInformations = true;
    }
    else if (g_CommandLine.buildCfg == "fast-debug")
    {
        buildCfg.byteCodeOptimizeLevel    = 1;
        buildCfg.byteCodeInline           = true;
        buildCfg.byteCodeAutoInline       = true;
        buildCfg.byteCodeEmitAssume       = true;
        buildCfg.safetyGuards             = SAFETY_ALL & ~SAFETY_NAN & ~SAFETY_BOOL;
        buildCfg.errorStackTrace          = true;
        buildCfg.debugAllocator           = true;
        buildCfg.backendOptimize          = BuildCfgBackendOptim::O2;
        buildCfg.backendDebugInformations = true;
    }
    else if (g_CommandLine.buildCfg == "release")
    {
        buildCfg.byteCodeOptimizeLevel          = 2;
        buildCfg.byteCodeInline                 = true;
        buildCfg.byteCodeAutoInline             = true;
        buildCfg.byteCodeEmitAssume             = false;
        buildCfg.safetyGuards                   = 0;
        buildCfg.errorStackTrace                = false;
        buildCfg.debugAllocator                 = false;
        buildCfg.backendOptimize                = BuildCfgBackendOptim::O3;
        buildCfg.backendDebugInformations       = true;
        buildCfg.backendLLVM.fpMathFma          = true;
        buildCfg.backendLLVM.fpMathNoInf        = true;
        buildCfg.backendLLVM.fpMathNoNaN        = true;
        buildCfg.backendLLVM.fpMathNoSignedZero = true;
    }

    // Overwrite with command line
    if (g_CommandLine.buildCfgInlineBC != "default")
        buildCfg.byteCodeInline = g_CommandLine.buildCfgInlineBC == "true" ? true : false;
    if (g_CommandLine.buildCfgOptimBC != "default")
        buildCfg.byteCodeOptimizeLevel = max(0, min(atoi(g_CommandLine.buildCfgOptimBC.c_str()), 2));
    if (g_CommandLine.buildCfgDebug != "default")
        buildCfg.backendDebugInformations = g_CommandLine.buildCfgDebug == "true" ? true : false;
    if (g_CommandLine.buildCfgOptim != "default")
        buildCfg.backendOptimize = (BuildCfgBackendOptim) max(0, min(atoi(g_CommandLine.buildCfgOptim.c_str()), 5));
    if (g_CommandLine.buildCfgSafety != "default")
        buildCfg.safetyGuards = g_CommandLine.buildCfgSafety == "true" ? SAFETY_ALL : 0;
    if (g_CommandLine.buildCfgStackTrace != "default")
        buildCfg.errorStackTrace = g_CommandLine.buildCfgStackTrace == "true" ? true : false;
    if (g_CommandLine.buildCfgDebugAlloc != "default")
        buildCfg.debugAllocator = g_CommandLine.buildCfgDebugAlloc == "true" ? true : false;
    if (g_CommandLine.buildCfgLlvmIR != "default")
        buildCfg.backendLLVM.outputIR = g_CommandLine.buildCfgLlvmIR == "true" ? true : false;

    if (!g_CommandLine.docCss.empty())
    {
        buildCfg.genDoc.css.buffer = g_CommandLine.docCss.buffer;
        buildCfg.genDoc.css.count  = g_CommandLine.docCss.count;
    }

    computePublicPath();
}

void Module::release()
{
    cacheSourceLoc.release();
    typeGen.release();

    // This is a problem because of premain called in bytecode, and type registration
    // constantSegment.release();
    // compilerSegment.release();

    mutableSegment.release();
    bssSegment.release();
    tlsSegment.release();
    for (auto c : compilerSegmentPerThread)
        c->release();

    for (auto& b : byteCodeFunc)
        b->release();

    // In case of errors, it's too tricky for now to release ASTs, because of possible shared values (like scopes).
    if (!isErrorModule && !numErrors)
    {
        for (auto f : files)
            f->release();
    }
}

void Module::computePublicPath()
{
    if (kind == ModuleKind::BootStrap || kind == ModuleKind::Runtime)
        return;
    if (path.empty())
        return;

    publicPath = path;
    publicPath.append(SWAG_PUBLIC_FOLDER);

    if (!isScriptFile && kind != ModuleKind::Script && !isErrorModule)
    {
        error_code err;
        if (!filesystem::exists(publicPath, err))
        {
            if (!filesystem::create_directories(publicPath, err))
            {
                Report::errorOS(Fmt(Err(Fat0019), publicPath.string().c_str()));
                OS::exit(-1);
            }
        }
    }

    publicPath.append(g_Workspace->getTargetFullName(g_CommandLine.buildCfg, g_CommandLine.target).c_str());

    if (!isScriptFile && kind != ModuleKind::Script && !isErrorModule)
    {
        error_code err;
        if (!filesystem::exists(publicPath, err))
        {
            if (!filesystem::create_directories(publicPath, err))
            {
                Report::errorOS(Fmt(Err(Fat0019), publicPath.string().c_str()));
                OS::exit(-1);
            }
        }
    }
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
        for (uint32_t i = 0; i < name.length(); i++)
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
                reason = Fmt("forbidden character [[%c]]", name[i]);
                break;
            }
        }
    }

    if (error)
    {
        errorStr = Fmt("invalid module name [[%s]], ", name.c_str());
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

    typeGen.initFrom(this, &other->typeGen);
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

void Module::buildGlobalVarsToDropSlice()
{
    if (kind == ModuleKind::Config || kind == ModuleKind::BootStrap || kind == ModuleKind::Runtime)
        return;
    if (globalVarsToDrop.size() == 0)
        return;

    uint8_t* resultPtr;
    globalVarsToDropSliceOffset = mutableSegment.reserve((uint32_t) globalVarsToDrop.size() * sizeof(SwagGlobalVarToDrop), &resultPtr);
    globalVarsToDropSlice       = (SwagGlobalVarToDrop*) resultPtr;
    auto offset                 = globalVarsToDropSliceOffset;

    auto oneVar = globalVarsToDropSlice;
    for (auto& g : globalVarsToDrop)
    {
        // Variable address
        oneVar->ptr = g.storageSegment->address(g.storageOffset);
        mutableSegment.addInitPtr(offset, g.storageOffset, g.storageSegment->kind);

        // opDrop function
        if (g.type->opUserDropFct && g.type->opUserDropFct->isForeign())
        {
            g.type->opUserDropFct->computeFullNameForeign(false);
            mutableSegment.addInitPtrFunc(offset + sizeof(void*), g.type->opUserDropFct->fullnameForeign);
        }
        else
        {
            SWAG_ASSERT(g.type->opDrop);
            auto opDrop     = g.type->opDrop;
            oneVar->opDrop  = opDrop;
            opDrop->isInSeg = true;

            if (opDrop->node)
            {
                auto funcNode = CastAst<AstFuncDecl>(opDrop->node, AstNodeKind::FuncDecl);
                mutableSegment.addInitPtrFunc(offset + sizeof(void*), funcNode->getCallName());
            }
            else
            {
                mutableSegment.addInitPtrFunc(offset + sizeof(void*), opDrop->getCallName());
            }
        }

        oneVar->sizeOf = g.type->sizeOf;
        oneVar->count  = g.count;

        oneVar++;
        offset += sizeof(SwagGlobalVarToDrop);
    }
}

void Module::buildTypesSlice()
{
    if (modulesSliceOffset == UINT32_MAX)
        return;

    auto&    mapTypes = typeGen.getMapPerSeg(&constantSegment).exportedTypes;
    uint8_t* resultPtr;
    uint32_t numTypes = (uint32_t) mapTypes.size();

    typesSliceOffset = constantSegment.reserve(sizeof(uint64_t) + (numTypes * sizeof(ExportedTypeInfo*)), &resultPtr);
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

    for (auto t : mapTypes)
    {
        *(ExportedTypeInfo**) resultPtr = t.second.exportedType;
        constantSegment.addInitPtr(offset, t.second.storageOffset);

        resultPtr += sizeof(ExportedTypeInfo*);
        offset += sizeof(ExportedTypeInfo*);
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

SourceFile* Module::findFile(const Utf8& fileName)
{
    for (auto p : files)
    {
        if (p->path == fileName.c_str())
            return p;
        if (p->name == fileName.c_str())
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
    if (!shouldHaveError &&
        !shouldHaveWarning &&
        buildPass >= BuildPass::Backend &&
        kind != ModuleKind::Runtime &&
        kind != ModuleKind::BootStrap)
    {
        switch (g_CommandLine.backendGenType)
        {
        case BackendGenType::LLVM:
            backend = new LLVM(this);
            break;
        case BackendGenType::SCBE:
            backend = new SCBE(this);
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

void Module::inheritCfgFrom(Module* from)
{
    buildCfg.warnAsErrors        = from->buildCfg.warnAsErrors;
    buildCfg.warnAsWarnings      = from->buildCfg.warnAsWarnings;
    buildCfg.warnAsDisabled      = from->buildCfg.warnAsDisabled;
    buildCfg.warnDefaultDisabled = from->buildCfg.warnDefaultDisabled;
    buildCfg.warnDefaultErrors   = from->buildCfg.warnDefaultErrors;
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
        file->writeTime = OS::getFileWriteTime(file->path.string().c_str());
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

void Module::addGlobalVarToDrop(AstNode* node, uint32_t storageOffset, DataSegment* storageSegment)
{
    ScopedLock lk(mutexGlobalVars);

    auto typeNode = node->typeInfo;
    SWAG_ASSERT(typeNode);
    SWAG_ASSERT(typeNode->isStruct() || typeNode->isArrayOfStruct());

    uint32_t count = 1;
    if (typeNode->isArrayOfStruct())
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeNode, TypeInfoKind::Array);
        typeNode       = typeArray->finalType;
        count          = typeArray->totalCount;
    }

    TypeInfoStruct* typeStruct = CastTypeInfo<TypeInfoStruct>(typeNode, TypeInfoKind::Struct);
    SWAG_ASSERT(typeStruct->opDrop || (typeStruct->opUserDropFct && typeStruct->opUserDropFct->isForeign()));
    globalVarsToDrop.push_back({typeStruct, storageOffset, storageSegment, count});
}

void Module::addCompilerFunc(ByteCode* bc)
{
    auto funcDecl = CastAst<AstFuncDecl>(bc->node, AstNodeKind::FuncDecl);

    SWAG_ASSERT(funcDecl->parameters);
    SWAG_ASSERT(funcDecl->parameters->hasComputedValue());

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

    SWAG_ASSERT(!bc->node || !bc->node->isForeign());
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
        {
            bc->node->ownerScope->addPublicFunc(bc->node);
        }

        if (attributeFlags & ATTRIBUTE_TEST_FUNC)
        {
            if (g_CommandLine.testFilter.empty() || bc->node->sourceFile->name.containsNoCase(g_CommandLine.testFilter))
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

bool Module::addFileToLoad(AstNode* includeNode)
{
    ScopedLock lk(mutexDependency);
    compilerLoads.push_back(includeNode);
    return true;
}

bool Module::removeFileToLoad(AstNode* includeNode)
{
    ScopedLock lk(mutexDependency);
    compilerLoads.erase_unordered_byval(includeNode);
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
                Diagnostic diag{importNode, tokenLocation, Fmt(Err(Err0066), dep->name.c_str(), dep->location.c_str())};
                auto       note = Diagnostic::note(dep->node, Nte(Nte0073));
                return Report::report(diag, note);
            }

            if (dep->version != tokenVersion.text && !tokenVersion.text.empty() && !dep->version.empty())
            {
                Diagnostic diag{importNode, tokenVersion, Fmt(Err(Err0067), dep->name.c_str(), dep->version.c_str())};
                auto       note = Diagnostic::note(dep->node, Nte(Nte0073));
                return Report::report(diag, note);
            }

            return true;
        }
    }

    ModuleDependency* dep = Allocator::alloc<ModuleDependency>();
    dep->node             = importNode;
    dep->name             = importNode->token.text;
    dep->location         = tokenLocation.text;
    dep->version          = tokenVersion.text.empty() ? "?.?.?" : tokenVersion.text;
    dep->tokenLocation    = tokenLocation;
    dep->tokenVersion     = tokenVersion;
    moduleDependencies.push_front(dep);

    // Check version
    Vector<Utf8> splits;
    Utf8::tokenize(dep->version, '.', splits);

    if (splits.size() != 3 || splits[0].empty() || splits[1].empty() || splits[2].empty())
    {
        Diagnostic diag{importNode, tokenVersion, Err(Err0312)};
        auto       note = Diagnostic::note(Nte(Nte0142));
        return Report::report(diag, note);
    }

    uint32_t* setVer = nullptr;
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
            *setVer = UINT32_MAX;
            continue;
        }

        // Be sure we have a number
        for (uint32_t j = 0; j < splits[i].length(); j++)
        {
            if (!isdigit(splits[i][j]))
            {
                Diagnostic diag{importNode, tokenVersion, Err(Err0312)};
                auto       note = Diagnostic::note(Err(Nte0142));
                return Report::report(diag, note);
            }
        }

        *setVer = atoi(splits[i]);
        if (*setVer < 0)
        {
            Diagnostic diag{importNode, tokenVersion, Err(Err0312)};
            auto       note = Diagnostic::note(Err(Nte0142));
            return Report::report(diag, note);
        }

        switch (i)
        {
        case 1:
            SWAG_VERIFY(dep->verNum != UINT32_MAX, Report::report({importNode, tokenVersion, Fmt(Err(Err0127), dep->revNum)}));
            break;
        case 2:
            SWAG_VERIFY(dep->revNum != UINT32_MAX, Report::report({importNode, tokenVersion, Fmt(Err(Err0126), dep->buildNum)}));
            break;
        }
    }

    return true;
}

bool Module::removeDependency(AstNode* importNode)
{
    ScopedLock lk(mutexDependency);
    for (size_t i = 0; i < moduleDependencies.size(); i++)
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

bool Module::waitForDependenciesDone(Job* job, const SetUtf8& modules)
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
            job->setPendingInfos(JobWaitKind::DepDone);
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
            job->setPendingInfos(JobWaitKind::DepDone);
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
    buildPass = (BuildPass) min((int) g_CommandLine.buildPass, (int) buildPass);
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

void Module::startBuilding(const BuildParameters& bp)
{
    if (!backend->mustCompile)
    {
        g_Workspace->skippedModules += 1;
    }
}

void Module::printBC()
{
    ByteCodePrintOptions opt;
    for (auto bc : byteCodePrintBC)
        bc->print(opt);
}

bool Module::mustEmitSafetyOverflow(AstNode* node, bool compileTime)
{
    return mustEmitSafety(node, SAFETY_OVERFLOW, compileTime);
}

bool Module::mustEmitSafety(AstNode* node, uint16_t what, bool compileTime)
{
    if (what == SAFETY_OVERFLOW)
    {
        if (node->attributeFlags & ATTRIBUTE_CAN_OVERFLOW_ON)
            return false;
    }

    if (node->safetyOff & what)
        return false;
    if (node->safetyOn & what)
        return true;

    // At compile time, we must emit safety except if the user has specifically changed it in the code
    if (compileTime)
        return true;

    return (buildCfg.safetyGuards & what);
}

bool Module::mustOptimizeBytecode(AstNode* node)
{
    if (!node)
        return buildCfg.byteCodeOptimizeLevel > 0;

    while (node)
    {
        if (node->attributeFlags & ATTRIBUTE_OPTIM_BYTECODE_OFF)
            return false;
        if (node->attributeFlags & ATTRIBUTE_OPTIM_BYTECODE_ON)
            return true;
        node = node->ownerFct;
    }

    return buildCfg.byteCodeOptimizeLevel > 0;
}

bool Module::mustOptimizeBackend(AstNode* node)
{
    if (!node)
        return buildCfg.byteCodeOptimizeLevel > 0;
    return (buildCfg.byteCodeOptimizeLevel > 0 || (node->attributeFlags & ATTRIBUTE_OPTIM_BACKEND_ON)) && !(node->attributeFlags & ATTRIBUTE_OPTIM_BACKEND_OFF);
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

bool Module::mustGenerateTestExe()
{
    if (!g_CommandLine.test)
        return false;
    if (!g_CommandLine.outputTest)
        return false;
    if (kind != ModuleKind::Test)
        return false;
    if (buildCfg.backendKind != BuildCfgBackendKind::Executable)
        return false;
    if (g_CommandLine.scriptMode)
        return false;
    if (g_Workspace->filteredModule && g_Workspace->filteredModule != this)
        return false;
    if (shouldHaveError || shouldHaveWarning)
        return false;

    return true;
}

bool Module::mustGenerateLegit()
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
        if (buildCfg.backendKind == BuildCfgBackendKind::Executable)
            return false;
        if (g_CommandLine.scriptMode)
            return false;
    }

    return true;
}

bool Module::mustOutputSomething()
{
    bool mustOutput = true;
    // do not generate an executable that has been run in script mode
    if (byteCodeMainFunc && g_CommandLine.scriptMode)
        mustOutput = false;
    else if (kind == ModuleKind::BootStrap || kind == ModuleKind::Runtime)
        mustOutput = false;
    else if (buildPass < BuildPass::Backend)
        mustOutput = false;
    else if (files.empty())
        mustOutput = false;
    // a test module needs swag to be in test mode
    else if (kind == ModuleKind::Test && !g_CommandLine.outputTest)
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

    SWAG_ASSERT(g_RunContext->callerContext->baseJob);
    SWAG_ASSERT(g_RunContext->ip);
    SWAG_ASSERT(g_RunContext->ip->node);
    SWAG_ASSERT(g_RunContext->ip->node->sourceFile);

    auto ip         = g_RunContext->ip != g_RunContext->bc->out ? g_RunContext->ip - 1 : g_RunContext->ip;
    auto sourceFile = ip->node->sourceFile;

    // Is it still possible to generate some code ?
    if (!acceptsCompileString)
    {
        Report::report({ip->node, ip->node->token, Err(Err0112)});
        return false;
    }

    auto parent = Ast::newNode<AstNode>(nullptr, AstNodeKind::StatementNoScope, files[0], sourceFile->astRoot);

    JobContext jobContext;
    Parser     parser;
    parser.setup(&jobContext, this, sourceFile);
    if (!parser.constructEmbeddedAst(text, parent, ip->node, CompilerAstKind::TopLevelInstruction, true))
        return false;

    auto dependentJob = g_RunContext->callerContext->baseJob;
    while (dependentJob && dependentJob->dependentJob)
        dependentJob = dependentJob->dependentJob;

    SWAG_ASSERT(files[0]->module == this);
    SemanticJob::newJob(dependentJob, files[0], parent, true);
    return true;
}

TypeInfoFuncAttr* Module::getRuntimeTypeFct(const Utf8& fctName)
{
    SharedLock lk(mutexFile);

    auto it = mapRuntimeFcts.find(fctName);
    if (it != mapRuntimeFcts.end())
        return it->second->typeInfoFunc;

    auto it1 = mapRuntimeFctsTypes.find(fctName);
    if (it1 != mapRuntimeFctsTypes.end())
        return it1->second;
    return nullptr;
}

ByteCode* Module::getRuntimeFct(const Utf8& fctName)
{
    SharedLock lk(mutexFile);

    auto it = mapRuntimeFcts.find(fctName);
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

bool Module::filterFunctionsToEmit()
{
    byteCodeFuncToGen.reserve((int) byteCodeFunc.size());
    for (auto bc : byteCodeFunc)
    {
        SWAG_CHECK(SemanticError::warnUnusedFunction(this, bc));
        if (!bc->canEmit())
            continue;
        byteCodeFuncToGen.push_back(bc);
    }

    sort(byteCodeFuncToGen.begin(), byteCodeFuncToGen.end(), [](ByteCode* bc1, ByteCode* bc2)
         { return (size_t) bc1->sourceFile < (size_t) bc2->sourceFile; });

    return true;
}

void Module::flushGenFiles()
{
    auto newJob    = Allocator::alloc<SaveGenJob>();
    newJob->module = this;
    g_ThreadMgr.addJob(newJob);
}

void Module::logStage(const char* msg)
{
    if (!g_CommandLine.verboseStages)
        return;
    g_Log.messageVerbose(Fmt("[%s] -- %s", name.c_str(), msg));
}

void Module::logPass(ModuleBuildPass pass)
{
    if (curPass == pass)
        return;
    curPass = pass;

    if (isErrorModule ||
        kind == ModuleKind::Config ||
        kind == ModuleKind::Runtime ||
        kind == ModuleKind::BootStrap ||
        buildCfg.backendKind == BuildCfgBackendKind::Export)
        return;

    Utf8 str;
    switch (pass)
    {
    case ModuleBuildPass::Init:
        // str = "Init";
        break;
    case ModuleBuildPass::Dependencies:
        // str = "Dependencies";
        break;
    case ModuleBuildPass::Syntax:
        // str = "Syntax";
        break;
    case ModuleBuildPass::IncludeSwg:
        // str = "IncludeSwg";
        break;
    case ModuleBuildPass::BeforeCompilerMessagesPass0:
        // str = "BeforeCompilerMessagesPass0";
        break;
    case ModuleBuildPass::CompilerMessagesPass0:
        // str = "CompilerMessagesPass0";
        break;
    case ModuleBuildPass::BeforeCompilerMessagesPass1:
        // str = "BeforeCompilerMessagesPass1";
        break;
    case ModuleBuildPass::AfterSemantic:
        // str = "AfterSemantic";
        break;
    case ModuleBuildPass::WaitForDependencies:
        // str = "WaitForDependencies";
        break;
    case ModuleBuildPass::FlushGenFiles:
        // str = "FlushGenFiles";
        break;
    case ModuleBuildPass::OptimizeBc:
        // str = "Optimizing";
        break;
    case ModuleBuildPass::Publish:
        // str = "Publishing";
        break;
    case ModuleBuildPass::GenerateDoc:
        str = "Documenting";
        break;
    case ModuleBuildPass::SemanticModule:
        str = "Compiling";
        break;
    case ModuleBuildPass::RunByteCode:
        if (kind == ModuleKind::Script)
            str = "Running ByteCode";
        break;
    case ModuleBuildPass::Output:
        if (backend->mustCompile && !g_CommandLine.genDoc)
            str = "Generating";
        break;
    case ModuleBuildPass::RunNative:
        if (mustGenerateTestExe() && g_CommandLine.runBackendTests)
            str = "Testing Backend";
        else
            str = "Running Backend";
        break;
    default:
        break;
    }

    if (str.empty())
        return;
    g_Log.messageHeaderCentered(str, name);
}