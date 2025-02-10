#include "pch.h"
#include "Wmf/Module.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/Context.h"
#include "Backend/LLVM/Main/LLVM.h"
#include "Backend/SCBE/Main/SCBE.h"
#include "Jobs/SaveGenJob.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Log.h"
#include "Report/Report.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/SemanticJob.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Parser/Parser.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Threading/ThreadManager.h"
#include "Wmf/ModuleManager.h"
#include "Wmf/Workspace.h"

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

    scopeRoot                = Ast::newScope(nullptr, "", ScopeKind::Module, nullptr);
    astRoot                  = Ast::newNode<AstNode>(AstNodeKind::Module, nullptr, nullptr);
    scopeRoot->owner         = astRoot;
    buildPass                = g_CommandLine.buildPass;
    buildParameters.buildCfg = &buildCfg;

    // Setup build configuration
    if (g_CommandLine.buildCfg == "fast-compile")
    {
        buildCfg.byteCodeOptimizeLevel = BuildCfgByteCodeOptim::O0;
        buildCfg.byteCodeInline        = false;
        buildCfg.byteCodeAutoInline    = false;
        buildCfg.byteCodeEmitAssume    = true;
        buildCfg.safetyGuards          = 0;
        buildCfg.sanity                = false;
        buildCfg.errorStackTrace       = false;
        buildCfg.debugAllocator        = true;
        buildCfg.backendOptimize       = BuildCfgBackendOptim::O0;
        buildCfg.backendDebugInfos     = false;
    }
    else if (g_CommandLine.buildCfg == "debug")
    {
        buildCfg.byteCodeOptimizeLevel = BuildCfgByteCodeOptim::O1;
        buildCfg.byteCodeInline        = false;
        buildCfg.byteCodeAutoInline    = false;
        buildCfg.byteCodeEmitAssume    = true;
        buildCfg.safetyGuards          = SAFETY_ALL;
        buildCfg.sanity                = true;
        buildCfg.errorStackTrace       = true;
        buildCfg.debugAllocator        = true;
        buildCfg.backendOptimize       = BuildCfgBackendOptim::O1;
        buildCfg.backendDebugInfos     = true;
        buildCfg.backendDebugInline    = true;
    }
    else if (g_CommandLine.buildCfg == "fast-debug")
    {
        buildCfg.byteCodeOptimizeLevel = BuildCfgByteCodeOptim::O2;
        buildCfg.byteCodeInline        = true;
        buildCfg.byteCodeAutoInline    = true;
        buildCfg.byteCodeEmitAssume    = true;
        buildCfg.safetyGuards          = SAFETY_ALL;
        buildCfg.safetyGuards.remove(SAFETY_NAN);
        buildCfg.safetyGuards.remove(SAFETY_BOOL);
        buildCfg.sanity            = true;
        buildCfg.errorStackTrace   = true;
        buildCfg.debugAllocator    = true;
        buildCfg.backendOptimize   = BuildCfgBackendOptim::O2;
        buildCfg.backendDebugInfos = true;
    }
    else if (g_CommandLine.buildCfg == "release")
    {
        buildCfg.byteCodeOptimizeLevel          = BuildCfgByteCodeOptim::O3;
        buildCfg.byteCodeInline                 = true;
        buildCfg.byteCodeAutoInline             = true;
        buildCfg.byteCodeEmitAssume             = false;
        buildCfg.safetyGuards                   = 0;
        buildCfg.sanity                         = false;
        buildCfg.errorStackTrace                = false;
        buildCfg.debugAllocator                 = false;
        buildCfg.backendOptimize                = BuildCfgBackendOptim::O3;
        buildCfg.backendDebugInfos              = true;
        buildCfg.backendLLVM.fpMathFma          = true;
        buildCfg.backendLLVM.fpMathNoInf        = true;
        buildCfg.backendLLVM.fpMathNoNaN        = true;
        buildCfg.backendLLVM.fpMathNoSignedZero = true;
    }

    // Overwrite with command line
    if (g_CommandLine.buildCfgInlineBC != "default")
        buildCfg.byteCodeInline = g_CommandLine.buildCfgInlineBC == "true";
    if (g_CommandLine.buildCfgDebug != "default")
        buildCfg.backendDebugInfos = g_CommandLine.buildCfgDebug == "true";
    if (g_CommandLine.buildCfgSafety != "default")
        buildCfg.safetyGuards = g_CommandLine.buildCfgSafety == "true" ? SAFETY_ALL : 0;
    if (g_CommandLine.buildCfgSanity != "default")
        buildCfg.sanity = g_CommandLine.buildCfgSanity == "true";
    if (g_CommandLine.buildCfgStackTrace != "default")
        buildCfg.errorStackTrace = g_CommandLine.buildCfgStackTrace == "true";
    if (g_CommandLine.buildCfgDebugAlloc != "default")
        buildCfg.debugAllocator = g_CommandLine.buildCfgDebugAlloc == "true";
    if (g_CommandLine.buildCfgLlvmIR != "default")
        buildCfg.backendLLVM.outputIR = g_CommandLine.buildCfgLlvmIR == "true";

    if (g_CommandLine.buildCfgOptimBC != "default")
    {
        if (g_CommandLine.buildCfgOptimBC == "O0")
            buildCfg.byteCodeOptimizeLevel = BuildCfgByteCodeOptim::O0;
        else if (g_CommandLine.buildCfgOptimBC == "O1")
            buildCfg.byteCodeOptimizeLevel = BuildCfgByteCodeOptim::O1;
        else if (g_CommandLine.buildCfgOptimBC == "O2")
            buildCfg.byteCodeOptimizeLevel = BuildCfgByteCodeOptim::O2;
        else if (g_CommandLine.buildCfgOptimBC == "O3")
            buildCfg.byteCodeOptimizeLevel = BuildCfgByteCodeOptim::O3;
    }

    if (g_CommandLine.buildCfgOptim != "default")
    {
        if (g_CommandLine.buildCfgOptim == "O0")
            buildCfg.backendOptimize = BuildCfgBackendOptim::O0;
        else if (g_CommandLine.buildCfgOptim == "O1")
            buildCfg.backendOptimize = BuildCfgBackendOptim::O1;
        else if (g_CommandLine.buildCfgOptim == "O2")
            buildCfg.backendOptimize = BuildCfgBackendOptim::O2;
        else if (g_CommandLine.buildCfgOptim == "O3")
            buildCfg.backendOptimize = BuildCfgBackendOptim::O3;
        else if (g_CommandLine.buildCfgOptim == "Os")
            buildCfg.backendOptimize = BuildCfgBackendOptim::Os;
        else if (g_CommandLine.buildCfgOptim == "Oz")
            buildCfg.backendOptimize = BuildCfgBackendOptim::Oz;
    }

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
    for (const auto c : compilerSegmentPerThread)
        c->release();

    for (const auto& b : byteCodeFunc)
        b->release();

    // In case of errors, it's too tricky for now to release ASTs, because of possible shared values (like scopes).
    if (!isErrorModule && !numErrors)
    {
        for (const auto f : files)
            f->release();
    }
}

void Module::computePublicPath()
{
    if (is(ModuleKind::BootStrap) || is(ModuleKind::Runtime) || is(ModuleKind::Fake))
        return;
    if (path.empty())
        return;

    publicPath = path;
    publicPath.append(SWAG_PUBLIC_FOLDER);

    if (!isScriptFile && isNot(ModuleKind::Script) && !isErrorModule)
    {
        std::error_code err;
        if (!std::filesystem::exists(publicPath, err))
        {
            if (!std::filesystem::create_directories(publicPath, err))
            {
                Report::errorOS(formErr(Fat0019, publicPath.cstr()));
                OS::exit(-1);
            }
        }
    }

    publicPath.append(Workspace::getTargetFullName(g_CommandLine.buildCfg, g_CommandLine.target).cstr());

    if (!isScriptFile && isNot(ModuleKind::Script) && !isErrorModule)
    {
        std::error_code err;
        if (!std::filesystem::exists(publicPath, err))
        {
            if (!std::filesystem::create_directories(publicPath, err))
            {
                Report::errorOS(formErr(Fat0019, publicPath.cstr()));
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
    else if (Parser::isGeneratedName(name))
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
        for (const char i : name)
        {
            if (i <= 32)
            {
                error  = true;
                reason = "name cannot contain blank characters";
                break;
            }

            if (!isalnum(i) && i != '_')
            {
                error  = true;
                reason = form("forbidden character [[%c]]", i);
                break;
            }
        }
    }

    if (error)
    {
        errorStr = form("invalid module name [[%s]], ", name.cstr());
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
    if (is(ModuleKind::Config) || is(ModuleKind::BootStrap) || is(ModuleKind::Runtime))
        return;

    uint8_t* resultPtr;
    modulesSliceOffset = constantSegment.reserve((moduleDependencies.count + 1) * sizeof(SwagModule), &resultPtr);
    modulesSlice       = reinterpret_cast<SwagModule*>(resultPtr);
    auto offset        = modulesSliceOffset;

    // Module name
    uint8_t* str;
    auto     offsetStr = constantSegment.addString(name, &str);
    constantSegment.addInitPtr(offset, offsetStr);
    *reinterpret_cast<uint8_t**>(resultPtr) = str;
    resultPtr += sizeof(void*);
    offset += sizeof(void*);
    *reinterpret_cast<uint64_t*>(resultPtr) = name.length();
    resultPtr += sizeof(void*);
    offset += sizeof(void*);

    // Slice of types
    *reinterpret_cast<uint8_t**>(resultPtr) = nullptr;
    resultPtr += sizeof(void*);
    offset += sizeof(void*);
    *reinterpret_cast<uint64_t*>(resultPtr) = 0;
    resultPtr += sizeof(void*);
    offset += sizeof(void*);

    for (const auto& dep : moduleDependencies)
    {
        // Module name
        offsetStr = constantSegment.addString(dep->module->name, &str);
        constantSegment.addInitPtr(offset, offsetStr);
        *reinterpret_cast<uint8_t**>(resultPtr) = str;
        resultPtr += sizeof(void*);
        offset += sizeof(void*);
        *reinterpret_cast<uint64_t*>(resultPtr) = dep->module->name.length();
        resultPtr += sizeof(void*);
        offset += sizeof(void*);

        // Slice of types
        *reinterpret_cast<uint8_t**>(resultPtr) = nullptr;
        resultPtr += sizeof(void*);
        offset += sizeof(void*);
        *reinterpret_cast<uint64_t*>(resultPtr) = 0;
        resultPtr += sizeof(void*);
        offset += sizeof(void*);
    }

    SWAG_ASSERT(offset - modulesSliceOffset == (moduleDependencies.count + 1) * sizeof(SwagModule));
}

void Module::buildGlobalVarsToDropSlice()
{
    if (is(ModuleKind::Config) || is(ModuleKind::BootStrap) || is(ModuleKind::Runtime))
        return;
    if (globalVarsToDrop.empty())
        return;

    uint8_t* resultPtr;
    globalVarsToDropSliceOffset = mutableSegment.reserve(globalVarsToDrop.size() * sizeof(SwagGlobalVarToDrop), &resultPtr);
    globalVarsToDropSlice       = reinterpret_cast<SwagGlobalVarToDrop*>(resultPtr);
    auto offset                 = globalVarsToDropSliceOffset;

    auto oneVar = globalVarsToDropSlice;
    for (const auto& g : globalVarsToDrop)
    {
        // Variable address
        oneVar->ptr = g.storageSegment->address(g.storageOffset);
        mutableSegment.addInitPtr(offset, g.storageOffset, g.storageSegment->kind);

        // opDrop function
        if (g.type->opUserDropFct && g.type->opUserDropFct->isForeign())
        {
            mutableSegment.addInitPtrFunc(offset + sizeof(void*), g.type->opUserDropFct->getFullNameForeignImport());
        }
        else
        {
            SWAG_ASSERT(g.type->opDrop);
            const auto opDrop       = g.type->opDrop;
            oneVar->opDrop          = opDrop;
            opDrop->isInDataSegment = true;

            if (opDrop->node)
            {
                const auto funcNode = castAst<AstFuncDecl>(opDrop->node, AstNodeKind::FuncDecl);
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

    const auto&    mapTypes = typeGen.getMapPerSeg(&constantSegment).exportedTypes;
    uint8_t*       resultPtr;
    const uint32_t numTypes = static_cast<uint32_t>(mapTypes.size());

    typesSliceOffset = constantSegment.reserve(sizeof(uint64_t) + numTypes * sizeof(ExportedTypeInfo*), &resultPtr);
    auto offset      = typesSliceOffset;

    // First store the number of types in the table
    *reinterpret_cast<uint64_t*>(resultPtr) = numTypes;
    resultPtr += sizeof(uint64_t);
    offset += sizeof(uint64_t);

    // Initialize the "current module" slice of types
    const auto moduleSlice    = reinterpret_cast<SwagModule*>(constantSegment.address(modulesSliceOffset));
    moduleSlice->types.buffer = resultPtr;
    moduleSlice->types.count  = numTypes;
    constantSegment.addInitPtr(modulesSliceOffset + offsetof(SwagModule, types), typesSliceOffset + sizeof(uint64_t));

    for (const auto& val : mapTypes | std::views::values)
    {
        *reinterpret_cast<ExportedTypeInfo**>(resultPtr) = val.exportedType;
        constantSegment.addInitPtr(offset, val.storageOffset);

        resultPtr += sizeof(ExportedTypeInfo*);
        offset += sizeof(ExportedTypeInfo*);
    }

    // Patch module list
    int i = 1;
    for (const auto& dep : moduleDependencies)
    {
        auto       callTable = dep->module->getGlobalPrivateFct(g_LangSpec->name_getTypeTable);
        const auto ptr       = g_ModuleMgr->getFnPointer(dep->module->name, callTable);
        if (!ptr)
        {
            moduleSlice[i].types.buffer = nullptr;
            moduleSlice[i].types.count  = 0;
        }
        else
        {
            using FuncCall    = uint8_t* (*) ();
            const auto valPtr = reinterpret_cast<FuncCall>(ptr)();

            moduleSlice[i].types.buffer = valPtr + sizeof(uint64_t);
            moduleSlice[i].types.count  = *reinterpret_cast<uint64_t*>(valPtr);
        }

        i += 1;
    }
}

SourceFile* Module::findFile(const Utf8& fileName)
{
    for (const auto p : files)
    {
        if (p->path == fileName.cstr())
            return p;
        if (p->name == fileName.cstr())
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
        isNot(ModuleKind::Runtime) &&
        isNot(ModuleKind::BootStrap))
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

DataSegment* Module::getSegment(SegmentKind segKind)
{
    switch (segKind)
    {
        case SegmentKind::Bss:
            return &bssSegment;
        case SegmentKind::Compiler:
            return &compilerSegment;
        case SegmentKind::Constant:
            return &constantSegment;
        case SegmentKind::Data:
            return &mutableSegment;
        case SegmentKind::Tls:
            return &tlsSegment;
    }

    SWAG_ASSERT(false);
    return nullptr;
}

void Module::inheritCfgFrom(const Module* from)
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
    file->indexInModule = files.size();
    files.push_back(file);

    // A file scope is not registered in the list of children of
    // its parent
    if (file->scopeFile)
        file->scopeFile->parentScope = scopeRoot;

    // Keep track of the most recent file
    if (!file->writeTime)
        file->writeTime = OS::getFileWriteTime(file->path);
    moreRecentSourceFile = max(moreRecentSourceFile, file->writeTime);

    // If the file is flagged as '#global export', register it
    if (file->hasFlag(FILE_FORCE_EXPORT))
        exportSourceFiles.insert(file);
}

void Module::removeFile(SourceFile* file)
{
    ScopedLock lk(mutexFile);
    ScopedLock lk1(scopeRoot->mutex);

    SWAG_ASSERT(file->module == this);

    const auto idx = file->indexInModule;
    SWAG_ASSERT(files[idx] == file);
    files[idx]                = files.back();
    files[idx]->indexInModule = idx;
    files.pop_back();
    file->module        = nullptr;
    file->indexInModule = UINT32_MAX;

    // If the file is flagged as '#global export', unregister it
    const auto it1 = exportSourceFiles.find(file);
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

void Module::addGlobalVarToDrop(const AstNode* node, uint32_t storageOffset, DataSegment* storageSegment)
{
    ScopedLock lk(mutexGlobalVars);

    auto typeNode = node->typeInfo;
    SWAG_ASSERT(typeNode);
    SWAG_ASSERT(typeNode->isStruct() || typeNode->isArrayOfStruct());

    uint32_t count = 1;
    if (typeNode->isArrayOfStruct())
    {
        const auto typeArray = castTypeInfo<TypeInfoArray>(typeNode, TypeInfoKind::Array);
        typeNode             = typeArray->finalType;
        count                = typeArray->totalCount;
    }

    TypeInfoStruct* typeStruct = castTypeInfo<TypeInfoStruct>(typeNode, TypeInfoKind::Struct);
    SWAG_ASSERT(typeStruct->opDrop || (typeStruct->opUserDropFct && typeStruct->opUserDropFct->isForeign()));
    globalVarsToDrop.push_back({typeStruct, storageOffset, storageSegment, count});
}

void Module::addCompilerFunc(ByteCode* bc)
{
    const auto funcDecl = castAst<AstFuncDecl>(bc->node, AstNodeKind::FuncDecl);

    SWAG_ASSERT(funcDecl->parameters);
    SWAG_ASSERT(funcDecl->parameters->hasFlagComputedValue());

    // Register the function in all the corresponding buckets
    const auto filter = funcDecl->parameters->computedValue()->reg.u64;
    for (uint32_t i = 0; i < 64; i++)
    {
        if (filter & static_cast<uint64_t>(1) << i)
        {
            ScopedLock lk(byteCodeCompilerMutex[i]);
            SWAG_ASSERT(numCompilerFunctions > 0);
            --numCompilerFunctions;
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
        const auto attributeFlags = bc->node->attributeFlags;
        const auto flags          = bc->node->flags;

        // Register for export
        if (attributeFlags.has(ATTRIBUTE_PUBLIC) &&
            !attributeFlags.has(ATTRIBUTE_INLINE) &&
            !attributeFlags.has(ATTRIBUTE_COMPILER) &&
            !attributeFlags.has(ATTRIBUTE_INIT_FUNC) &&
            !attributeFlags.has(ATTRIBUTE_DROP_FUNC) &&
            !attributeFlags.has(ATTRIBUTE_PREMAIN_FUNC) &&
            !attributeFlags.has(ATTRIBUTE_TEST_FUNC) &&
            !flags.has(AST_FROM_GENERIC))
        {
            bc->node->ownerScope->addPublicFunc(bc->node);
        }

        if (attributeFlags.has(ATTRIBUTE_TEST_FUNC))
        {
            if (g_CommandLine.testFilter.empty() || bc->node->token.sourceFile->name.containsNoCase(g_CommandLine.testFilter))
                byteCodeTestFunc.push_back(bc);
        }
        else if (attributeFlags.has(ATTRIBUTE_INIT_FUNC))
        {
            byteCodeInitFunc.push_back(bc);
        }
        else if (attributeFlags.has(ATTRIBUTE_PREMAIN_FUNC))
        {
            byteCodePreMainFunc.push_back(bc);
        }
        else if (attributeFlags.has(ATTRIBUTE_DROP_FUNC))
        {
            byteCodeDropFunc.push_back(bc);
        }
        else if (attributeFlags.has(ATTRIBUTE_RUN_FUNC))
        {
            byteCodeRunFunc.push_back(bc);
        }
        else if (attributeFlags.has(ATTRIBUTE_MAIN_FUNC))
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
    compilerLoads.erase_unordered_by_val(includeNode);
    return true;
}

bool Module::addDependency(AstNode* importNode, const Token& tokenLocation, const Token& tokenVersion)
{
    ScopedLock lk(mutexDependency);
    for (const auto& dep : moduleDependencies)
    {
        if (dep->name == importNode->token.text)
        {
            if (dep->location != tokenLocation.text && !tokenLocation.text.empty() && !dep->location.empty())
            {
                Diagnostic err{importNode, tokenLocation, formErr(Err0715, dep->name.cstr(), dep->location.cstr())};
                err.addNote(dep->node, toNte(Nte0196));
                return Report::report(err);
            }

            if (dep->version != tokenVersion.text && !tokenVersion.text.empty() && !dep->version.empty())
            {
                Diagnostic err{importNode, tokenVersion, formErr(Err0717, dep->name.cstr(), dep->version.cstr())};
                err.addNote(dep->node, toNte(Nte0196));
                return Report::report(err);
            }

            return true;
        }
    }

    ModuleDependency* dep = Allocator::alloc<ModuleDependency>();
    dep->node             = importNode;
    dep->name             = importNode->token.text;
    dep->location         = tokenLocation.text;
    dep->version          = tokenVersion.text.empty() ? Utf8{"?.?.?"} : tokenVersion.text;
    dep->tokenLocation    = tokenLocation;
    dep->tokenVersion     = tokenVersion;
    moduleDependencies.push_front(dep);

    // Check version
    Vector<Utf8> splits;
    Utf8::tokenize(dep->version, '.', splits);

    if (splits.size() != 3 || splits[0].empty() || splits[1].empty() || splits[2].empty())
    {
        Diagnostic err{importNode, tokenVersion, toErr(Err0740)};
        err.addNote(toNte(Nte0152));
        return Report::report(err);
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
            default:
                break;
        }

        if (splits[i] == '?')
        {
            *setVer = UINT32_MAX;
            continue;
        }

        if (!Utf8::isNumber(splits[i]))
        {
            Diagnostic err{importNode, tokenVersion, toErr(Err0740)};
            err.addNote(toErr(Nte0152));
            return Report::report(err);
        }

        *setVer = splits[i].toInt();

        switch (i)
        {
            case 1:
                SWAG_VERIFY(dep->verNum != UINT32_MAX, Report::report({importNode, tokenVersion, formErr(Err0741, dep->revNum)}));
                break;
            case 2:
                SWAG_VERIFY(dep->revNum != UINT32_MAX, Report::report({importNode, tokenVersion, formErr(Err0742, dep->buildNum)}));
                break;
            default:
                break;
        }
    }

    return true;
}

bool Module::removeDependency(const AstNode* importNode)
{
    ScopedLock lk(mutexDependency);
    for (uint32_t i = 0; i < moduleDependencies.size(); i++)
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
    for (const auto dep : moduleDependencies)
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
    for (const auto& dep : moduleDependencies)
    {
        const auto depModule = dep->module;
        SWAG_ASSERT(depModule);

        if (depModule->numErrors)
            continue;
        if (!modules.contains(depModule->name))
            continue;

        ScopedLock lk(depModule->mutexDependency);
        if (depModule->hasBeenBuilt != BUILDRES_FULL)
        {
            job->setPendingInfos(JobWaitKind::DepDone);
            depModule->dependentJobs.add(job);
            return false;
        }

        (void) g_ModuleMgr->loadModule(depModule->name);
    }

    return true;
}

bool Module::waitForDependenciesDone(Job* job)
{
    if (dependenciesDone)
        return true;

    for (const auto& dep : moduleDependencies)
    {
        const auto depModule = dep->module;
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

        (void) g_ModuleMgr->loadModule(depModule->name);
    }

    dependenciesDone = true;
    return true;
}

void Module::sortDependenciesByInitOrder(VectorNative<ModuleDependency*>& result) const
{
    result = moduleDependencies;
    std::ranges::sort(result, [](const ModuleDependency* n1, const ModuleDependency* n2) {
        return n2->module->hasDependencyTo(n1->module);
    });
}

void Module::setBuildPass(BuildPass buildP)
{
    ScopedLock lk(mutexBuildPass);
    buildPass = static_cast<BuildPass>(min((int) buildP, (int) buildPass));
    buildPass = static_cast<BuildPass>(min((int) g_CommandLine.buildPass, (int) buildPass));
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

void Module::startBuilding() const
{
    if (!backend->mustCompile)
    {
        g_Workspace->skippedModules += 1;
    }
}

void Module::printBC()
{
    constexpr ByteCodePrintOptions opt;
    for (const auto bc : byteCodePrintBC)
        bc->print(opt);
}

bool Module::mustEmitSafetyOverflow(const AstNode* node, bool compileTime) const
{
    return mustEmitSafety(node, SAFETY_OVERFLOW, compileTime);
}

bool Module::mustEmitSafety(const AstNode* node, SafetyFlags what, bool compileTime) const
{
    if (what == SAFETY_OVERFLOW)
    {
        if (node->hasAttribute(ATTRIBUTE_CAN_OVERFLOW_ON))
            return false;
    }

    if (node->safetyOff.has(what))
        return false;
    if (node->safetyOn.has(what))
        return true;

    // at compile time, we must emit safety except if the user has specifically changed it in the code
    if (compileTime)
        return true;

    return buildCfg.safetyGuards.has(what);
}

bool Module::mustOptimizeSemantic(const AstNode* node) const
{
    if (!node)
        return buildCfg.byteCodeOptimizeLevel > BuildCfgByteCodeOptim::O1;

    while (node)
    {
        if (node->hasAttribute(ATTRIBUTE_OPTIM_BYTECODE_OFF))
            return false;
        if (node->hasAttribute(ATTRIBUTE_OPTIM_BYTECODE_ON))
            return true;
        node = node->ownerFct;
    }

    return buildCfg.byteCodeOptimizeLevel > BuildCfgByteCodeOptim::O1;
}

bool Module::mustOptimizeByteCode(const AstNode* node) const
{
    if (!node)
        return buildCfg.byteCodeOptimizeLevel != BuildCfgByteCodeOptim::O0;

    while (node)
    {
        if (node->hasAttribute(ATTRIBUTE_OPTIM_BYTECODE_OFF))
            return false;
        if (node->hasAttribute(ATTRIBUTE_OPTIM_BYTECODE_ON))
            return true;
        node = node->ownerFct;
    }

    return buildCfg.byteCodeOptimizeLevel != BuildCfgByteCodeOptim::O0;
}

bool Module::mustOptimizeBackend(const AstNode* node) const
{
    if (!node)
        return buildCfg.backendOptimize != BuildCfgBackendOptim::O0;

    while (node)
    {
        if (node->hasAttribute(ATTRIBUTE_OPTIM_BACKEND_OFF))
            return false;
        if (node->hasAttribute(ATTRIBUTE_OPTIM_BACKEND_ON))
            return true;
        node = node->ownerFct;
    }

    return buildCfg.backendOptimize != BuildCfgBackendOptim::O0;
}

bool Module::hasBytecodeToRun() const
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

bool Module::mustGenerateTestExe() const
{
    if (!g_CommandLine.test)
        return false;
    if (!g_CommandLine.outputTest)
        return false;
    if (isNot(ModuleKind::Test))
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

bool Module::mustGenerateLegit() const
{
    // Normal module
    if (isNot(ModuleKind::Test))
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

bool Module::mustOutputSomething() const
{
    bool mustOutput = true;
    // do not generate an executable that has been run in script mode
    if (byteCodeMainFunc && g_CommandLine.scriptMode)
        mustOutput = false;
    else if (is(ModuleKind::BootStrap) || is(ModuleKind::Runtime))
        mustOutput = false;
    else if (buildPass < BuildPass::Backend)
        mustOutput = false;
    else if (files.empty())
        mustOutput = false;
    // a test module needs swag to be in test mode
    else if (is(ModuleKind::Test) && !g_CommandLine.outputTest)
        mustOutput = false;
    // if all files are exported, then do not generate a module
    else if (buildCfg.backendKind == BuildCfgBackendKind::Export)
        mustOutput = false;
    else if (isNot(ModuleKind::Test) && buildCfg.backendKind == BuildCfgBackendKind::None)
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
    SWAG_ASSERT(g_RunContext->ip->node->token.sourceFile);

    const auto ip         = g_RunContext->ip != g_RunContext->bc->out ? g_RunContext->ip - 1 : g_RunContext->ip;
    const auto sourceFile = ip->node->token.sourceFile;

    // Is it still possible to generate some code ?
    if (!acceptsCompileString)
    {
        Report::report({ip->node, ip->node->token, toErr(Err0086)});
        return false;
    }

    const auto parent = Ast::newNode<AstStatement>(AstNodeKind::StatementNoScope, nullptr, sourceFile->astRoot);

    JobContext jobContext;
    Parser     parser;
    parser.setup(&jobContext, this, sourceFile, PARSER_DEFAULT);
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

    const auto it = mapRuntimeFct.find(fctName);
    if (it != mapRuntimeFct.end())
        return it->second->typeInfoFunc;

    const auto it1 = mapRuntimeFctTypes.find(fctName);
    if (it1 != mapRuntimeFctTypes.end())
        return it1->second;
    return nullptr;
}

ByteCode* Module::getRuntimeFct(const Utf8& fctName)
{
    SharedLock lk(mutexFile);

    const auto it = mapRuntimeFct.find(fctName);
    SWAG_ASSERT(it != mapRuntimeFct.end());
    return it->second;
}

void Module::addImplForToSolve(const TypeInfoStruct* typeStruct)
{
    ScopedLock lk(mutexFile);

    auto it = implForToSolve.find(typeStruct->structName);
    if (it == implForToSolve.end())
    {
        implForToSolve[typeStruct->structName] = {};
        it                                     = implForToSolve.find(typeStruct->structName);
    }

    it->second.count++;
}

bool Module::waitImplForToSolve(Job* job, const TypeInfoStruct* typeStruct)
{
    ScopedLock lk(mutexFile);

    if (typeStruct->declNode && typeStruct->declNode->hasAstFlag(AST_FROM_GENERIC))
        return false;
    const auto it = implForToSolve.find(typeStruct->structName);
    if (it == implForToSolve.end())
        return false;
    if (it->second.count == 0)
        return false;

    it->second.dependentJobs.add(job);
    return true;
}

void Module::decImplForToSolve(const TypeInfoStruct* typeStruct)
{
    ScopedLock lk(mutexFile);

    const auto it = implForToSolve.find(typeStruct->structName);
    SWAG_ASSERT(it != implForToSolve.end());
    SWAG_ASSERT(it->second.count != 0);
    it->second.count--;

    if (it->second.count == 0)
        it->second.dependentJobs.setRunning();
}

void Module::initProcessInfos()
{
    std::copy_n(&g_ProcessInfos, 1, &processInfos);
    processInfos.modules.buffer = modulesSlice;
    processInfos.modules.count  = moduleDependencies.count + 1;
}

void Module::callPreMain()
{
    for (const auto& dep : moduleDependencies)
    {
        if (!dep->module->isSwag)
            continue;
        auto       nameFct = dep->module->getGlobalPrivateFct(g_LangSpec->name_globalPreMain);
        const auto ptr     = g_ModuleMgr->getFnPointer(dep->name, nameFct);
        if (!ptr)
            continue;
        using FuncCall = void (*)(SwagProcessInfos*);
        reinterpret_cast<FuncCall>(ptr)(&processInfos);
    }
}

Utf8 Module::getGlobalPrivateFct(const Utf8& nameFct) const
{
    return form(nameFct.cstr(), nameNormalized.cstr());
}

bool Module::filterFunctionsToEmit()
{
    byteCodeFuncToGen.reserve(static_cast<int>(byteCodeFunc.size()));
    for (auto bc : byteCodeFunc)
    {
        SWAG_CHECK(SemanticError::warnUnusedFunction(this, bc));
        if (!bc->canEmit())
            continue;
        byteCodeFuncToGen.push_back(bc);
    }

    std::ranges::sort(byteCodeFuncToGen, [](ByteCode* bc1, ByteCode* bc2) {
        return reinterpret_cast<size_t>(bc1->sourceFile) < reinterpret_cast<size_t>(bc2->sourceFile);
    });

    return true;
}

void Module::flushGenFiles()
{
    const auto newJob = Allocator::alloc<SaveGenJob>();
    newJob->module    = this;
    g_ThreadMgr.addJob(newJob);
}

void Module::logStage(const char* msg) const
{
    if (!g_CommandLine.verboseStages)
        return;
    g_Log.messageVerbose(form("[%s] -- %s", name.cstr(), msg));
}

void Module::logPass(ModuleBuildPass pass)
{
    if (curPass == pass)
        return;
    curPass = pass;

    if (isErrorModule ||
        is(ModuleKind::Config) ||
        is(ModuleKind::Runtime) ||
        is(ModuleKind::BootStrap) ||
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
            if (is(ModuleKind::Script))
                str = "Running ByteCode";
            break;
        case ModuleBuildPass::Output:
            // if (backend->mustCompile && !g_CommandLine.genDoc)
            //    str = "Generating";
            break;
        case ModuleBuildPass::RunNative:
            if (mustGenerateTestExe() && g_CommandLine.runBackendTests)
                str = "Testing Backend";
            else
                str = "Running Backend";
            break;
    }

    if (str.empty())
        return;
    g_Log.messageHeaderCentered(str, name);
}
