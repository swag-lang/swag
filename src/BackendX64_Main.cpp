#include "pch.h"
#include "BackendX64.h"
#include "Module.h"
#include "ByteCode.h"
#include "Context.h"
#include "Report.h"
#include "LanguageSpec.h"
#include "TypeManager.h"

bool BackendX64::emitOS(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    concat.align(16);

    if (g_CommandLine.target.os == SwagTargetOs::Windows)
    {
        // int _DllMainCRTStartup(void*, int, void*)
        pp.getOrAddSymbol("_DllMainCRTStartup", CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset);
        pp.emit_Load64_Immediate(1, RAX);
        pp.emit_Ret();
        return true;
    }
    else
    {
        Report::error(module, Fmt(Err(Err0056), Backend::getOsName(g_CommandLine.target)));
        return false;
    }
}

bool BackendX64::emitMain(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    concat.align(16);
    auto startAddress = concat.totalCount();

    const char* entryPoint = nullptr;
    switch (g_CommandLine.target.os)
    {
    case SwagTargetOs::Windows:
        if (buildParameters.buildCfg->subBackendKind == BuildCfgSubBackendKind::Console)
            entryPoint = "mainCRTStartup";
        else
            entryPoint = "WinMainCRTStartup";
        break;
    default:
        Report::error(module, Fmt(Err(Err0056), Backend::getOsName(g_CommandLine.target)));
        return false;
    }

    auto symbolFuncIndex = pp.getOrAddSymbol(entryPoint, CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    auto coffFct         = registerFunction(pp, nullptr, symbolFuncIndex);

    auto beforeProlog = concat.totalCount();
    pp.emit_Sub32_RSP(40);
    auto                   sizeProlog = concat.totalCount() - beforeProlog;
    VectorNative<uint16_t> unwind;
    computeUnwind({}, {}, 40, sizeProlog, unwind);

    // Set default system allocator function
    SWAG_ASSERT(g_SystemAllocatorTable);
    auto bcAlloc = (ByteCode*) ByteCode::undoByteCodeLambda(((void**) g_SystemAllocatorTable)[0]);
    SWAG_ASSERT(bcAlloc);
    pp.emit_Symbol_RelocationAddr(RAX, pp.symDefaultAllocTable, 0);
    concat.addString3("\x48\x8d\x0d"); // lea rcx, qword ptr ????????[rip]
    emitSymbolRelocation(pp, bcAlloc->getCallName());
    pp.emit_Store64_Indirect(0, RCX, RAX);

    // mainContext.allocator.itable = &defaultAllocTable;
    pp.emit_Symbol_RelocationAddr(RCX, pp.symMC_mainContext_allocator_itable, 0);
    pp.emit_Store64_Indirect(0, RAX, RCX);

    // main context flags
    pp.emit_Symbol_RelocationAddr(RCX, pp.symMC_mainContext_flags, 0);
    uint64_t contextFlags = getDefaultContextFlags(module);
    pp.emit_Store64_Immediate(0, contextFlags, RCX);

    //__process_infos.contextTlsId = swag_runtime_tlsAlloc();
    pp.emit_Symbol_RelocationAddr(RDI, pp.symPI_contextTlsId, 0);
    emitInternalCall(pp, module, g_LangSpec->name__tlsAlloc, {}, 0);

    //__process_infos.modules
    pp.emit_Symbol_RelocationAddr(RCX, pp.symPI_modulesAddr, 0);
    pp.emit_Symbol_RelocationAddr(RAX, pp.symCSIndex, module->modulesSliceOffset);
    pp.emit_Store64_Indirect(0, RAX, RCX);
    pp.emit_Symbol_RelocationAddr(RAX, pp.symPI_modulesCount, 0);
    pp.emit_Store64_Immediate(0, module->moduleDependencies.count + 1, RAX);

    //__process_infos.args
    pp.emit_Clear64(RCX);
    pp.emit_Symbol_RelocationAddr(RAX, pp.symPI_argsAddr, 0);
    pp.emit_Store64_Indirect(0, RCX, RAX);
    pp.emit_Symbol_RelocationAddr(RAX, pp.symPI_argsCount, 0);
    pp.emit_Store64_Indirect(0, RCX, RAX);

    // Set main context
    pp.emit_Symbol_RelocationAddr(RAX, pp.symMC_mainContext, 0);
    pp.emit_Symbol_RelocationAddr(RCX, pp.symPI_defaultContext, 0);
    pp.emit_Store64_Indirect(0, RAX, RCX);

    // Set current backend as X64
    pp.emit_Symbol_RelocationAddr(RCX, pp.symPI_backendKind, 0);
    pp.emit_Store32_Immediate(0, (uint32_t) SwagBackendGenType::X64, RCX);

    // Set default context in TLS
    pp.pushParams.clear();
    pp.pushParams.push_back({X64PushParamType::RelocV, pp.symPI_contextTlsId});
    pp.pushParams.push_back({X64PushParamType::RelocV, pp.symPI_defaultContext});
    emitInternalCallExt(pp, module, g_LangSpec->name__tlsSetValue, pp.pushParams);

    // Setup runtime
    auto rtFlags = Backend::getRuntimeFlags(module);
    pp.pushParams.clear();
    pp.pushParams.push_back({X64PushParamType::Imm64, rtFlags});
    emitInternalCallExt(pp, module, g_LangSpec->name__setupRuntime, pp.pushParams);

    // Load all dependencies
    VectorNative<ModuleDependency*> moduleDependencies;
    module->sortDependenciesByInitOrder(moduleDependencies);
    for (int i = 0; i < moduleDependencies.size(); i++)
    {
        auto dep      = moduleDependencies[i];
        auto nameDown = dep->name;
        Ast::normalizeIdentifierName(nameDown);
        auto nameLib = nameDown;
        nameLib += Backend::getOutputFileExtension(g_CommandLine.target, BuildCfgBackendKind::DynamicLib);

        pp.pushParams.clear();
        pp.pushParams.push_back({X64PushParamType::GlobalString, (uint64_t) nameLib.c_str()});
        pp.pushParams.push_back({X64PushParamType::Imm, (uint64_t) nameLib.length()});
        emitInternalCallExt(pp, module, g_LangSpec->name__loaddll, pp.pushParams);
    }

    pp.pushParams.clear();
    pp.pushParams.push_back({X64PushParamType::RelocAddr, pp.symPI_processInfos});

    // Call to global init of all dependencies
    for (int i = 0; i < moduleDependencies.size(); i++)
    {
        auto dep = moduleDependencies[i];
        SWAG_ASSERT(dep->module);
        if (!dep->module->isSwag)
            continue;
        auto nameFct = dep->module->getGlobalPrivFct(g_LangSpec->name_globalInit);
        emitInternalCallExt(pp, module, nameFct, pp.pushParams, UINT32_MAX, g_TypeMgr->typeInfoModuleCall);
    }

    // Call to global init of this module
    auto thisInit = module->getGlobalPrivFct(g_LangSpec->name_globalInit);
    emitInternalCallExt(pp, module, thisInit, pp.pushParams, UINT32_MAX, g_TypeMgr->typeInfoModuleCall);

    // Call to global premain of all dependencies
    for (int i = 0; i < moduleDependencies.size(); i++)
    {
        auto dep = moduleDependencies[i];
        SWAG_ASSERT(dep->module);
        if (!dep->module->isSwag)
            continue;

        auto nameFct = dep->module->getGlobalPrivFct(g_LangSpec->name_globalPreMain);
        emitInternalCallExt(pp, module, nameFct, pp.pushParams, UINT32_MAX, g_TypeMgr->typeInfoModuleCall);
    }

    // Call to global premain of this module
    thisInit = module->getGlobalPrivFct(g_LangSpec->name_globalPreMain);
    emitInternalCallExt(pp, module, thisInit, pp.pushParams, UINT32_MAX, g_TypeMgr->typeInfoModuleCall);

    // Call to test functions
    if (buildParameters.compileType == BackendCompileType::Test && !module->byteCodeTestFunc.empty())
    {
        for (auto bc : module->byteCodeTestFunc)
        {
            auto node = bc->node;
            if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;
            emitCall(pp, bc->getCallName());
        }
    }

    // Call to main
    if (module->byteCodeMainFunc)
    {
        emitCall(pp, module->byteCodeMainFunc->getCallName());
    }

    // Call to global drop of this module
    auto thisDrop = module->getGlobalPrivFct(g_LangSpec->name_globalDrop);
    emitCall(pp, thisDrop);

    // Call to global drop of all dependencies
    for (int i = (int) moduleDependencies.size() - 1; i >= 0; i--)
    {
        auto dep = moduleDependencies[i];
        if (!dep->module->isSwag)
            continue;
        auto nameFct = dep->module->getGlobalPrivFct(g_LangSpec->name_globalDrop);
        emitCall(pp, nameFct);
    }

    emitCall(pp, g_LangSpec->name__closeRuntime);

    pp.emit_Clear64(RAX);
    pp.emit_Add32_RSP(40);
    pp.emit_Ret();

    uint32_t endAddress = concat.totalCount();
    registerFunction(coffFct, startAddress, endAddress, sizeProlog, unwind);
    return true;
}

bool BackendX64::emitGetTypeTable(const BuildParameters& buildParameters)
{
    if (buildParameters.buildCfg->backendKind != BuildCfgBackendKind::DynamicLib)
        return true;

    int         ct              = buildParameters.compileType;
    int         precompileIndex = buildParameters.precompileIndex;
    auto&       pp              = *perThread[ct][precompileIndex];
    auto&       concat          = pp.concat;
    const auto& cc              = g_CallConv[g_TypeMgr->typeInfoModuleCall->callConv];

    concat.align(16);
    auto startAddress = concat.totalCount();

    auto thisInit        = module->getGlobalPrivFct(g_LangSpec->name_getTypeTable);
    auto symbolFuncIndex = pp.getOrAddSymbol(thisInit, CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    auto coffFct         = registerFunction(pp, nullptr, symbolFuncIndex);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::DynamicLib)
        pp.directives += Fmt("/EXPORT:%s ", thisInit.c_str());

    auto beforeProlog = concat.totalCount();
    pp.emit_Sub32_RSP(40);
    auto                   sizeProlog = concat.totalCount() - beforeProlog;
    VectorNative<uint16_t> unwind;
    computeUnwind({}, {}, 40, sizeProlog, unwind);

    pp.emit_Add32_RSP(40);
    pp.emit_Symbol_RelocationAddr(cc.returnByRegisterInteger, pp.symCSIndex, module->typesSliceOffset);
    pp.emit_Ret();

    uint32_t endAddress = concat.totalCount();
    registerFunction(coffFct, startAddress, endAddress, sizeProlog, unwind);

    return true;
}

bool BackendX64::emitGlobalPreMain(const BuildParameters& buildParameters)
{
    int         ct              = buildParameters.compileType;
    int         precompileIndex = buildParameters.precompileIndex;
    auto&       pp              = *perThread[ct][precompileIndex];
    auto&       concat          = pp.concat;
    const auto& cc              = g_CallConv[g_TypeMgr->typeInfoModuleCall->callConv];

    concat.align(16);
    auto startAddress = concat.totalCount();

    auto thisInit        = module->getGlobalPrivFct(g_LangSpec->name_globalPreMain);
    auto symbolFuncIndex = pp.getOrAddSymbol(thisInit, CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    auto coffFct         = registerFunction(pp, nullptr, symbolFuncIndex);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::DynamicLib)
        pp.directives += Fmt("/EXPORT:%s ", thisInit.c_str());

    auto beforeProlog = concat.totalCount();
    pp.emit_Push(RDI);
    pp.emit_Sub32_RSP(48);
    auto                   sizeProlog = concat.totalCount() - beforeProlog;
    VectorNative<uint16_t> unwind;
    computeUnwind({}, {}, 48, sizeProlog, unwind);

    // Store first parameter on stack (process infos ptr)
    SWAG_ASSERT(cc.byRegisterCount >= 1);
    pp.emit_LoadAddress_Indirect(0, RDI, RSP);
    pp.emit_Store64_Indirect(0, cc.byRegisterInteger[0], RDI);

    // Copy process infos passed as a parameter to the process info struct of this module
    pp.pushParams.clear();
    pp.pushParams.push_back({X64PushParamType::RelocAddr, pp.symPI_processInfos});
    pp.pushParams.push_back({X64PushParamType::Reg, 0});
    pp.pushParams.push_back({X64PushParamType::Imm, sizeof(SwagProcessInfos)});
    emitInternalCallExt(pp, module, g_LangSpec->name_memcpy, pp.pushParams);

    // Call to #premain functions
    for (auto bc : module->byteCodePreMainFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        emitCall(pp, bc->getCallName());
    }

    pp.emit_Add32_RSP(48);
    pp.emit_Pop(RDI);
    pp.emit_Ret();

    uint32_t endAddress = concat.totalCount();
    registerFunction(coffFct, startAddress, endAddress, sizeProlog, unwind);
    return true;
}

bool BackendX64::emitGlobalInit(const BuildParameters& buildParameters)
{
    int         ct              = buildParameters.compileType;
    int         precompileIndex = buildParameters.precompileIndex;
    auto&       pp              = *perThread[ct][precompileIndex];
    auto&       concat          = pp.concat;
    const auto& cc              = g_CallConv[g_TypeMgr->typeInfoModuleCall->callConv];

    concat.align(16);
    auto startAddress = concat.totalCount();

    auto thisInit        = module->getGlobalPrivFct(g_LangSpec->name_globalInit);
    auto symbolFuncIndex = pp.getOrAddSymbol(thisInit, CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    auto coffFct         = registerFunction(pp, nullptr, symbolFuncIndex);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::DynamicLib)
        pp.directives += Fmt("/EXPORT:%s ", thisInit.c_str());

    auto beforeProlog = concat.totalCount();
    pp.emit_Push(RDI);
    pp.emit_Sub32_RSP(48);
    auto                   sizeProlog = concat.totalCount() - beforeProlog;
    VectorNative<uint16_t> unwind;
    computeUnwind({}, {}, 48, sizeProlog, unwind);

    // Store first parameter on stack (process infos ptr)
    SWAG_ASSERT(cc.byRegisterCount >= 1);
    pp.emit_LoadAddress_Indirect(0, RDI, RSP);
    pp.emit_Store64_Indirect(0, cc.byRegisterInteger[0], RDI);

    // Copy process infos passed as a parameter to the process info struct of this module
    pp.pushParams.clear();
    pp.pushParams.push_back({X64PushParamType::RelocAddr, pp.symPI_processInfos});
    pp.pushParams.push_back({X64PushParamType::Reg, 0});
    pp.pushParams.push_back({X64PushParamType::Imm, sizeof(SwagProcessInfos)});
    emitInternalCallExt(pp, module, g_LangSpec->name_memcpy, pp.pushParams);

    // Thread local storage
    pp.emit_Symbol_RelocationAddr(RDI, pp.symTls_threadLocalId, 0);
    emitInternalCallExt(pp, module, g_LangSpec->name__tlsAlloc, {}, 0);

    // Init type table slice for each dependency (by calling ???_getTypeTable)
    pp.emit_Symbol_RelocationAddr(RCX, pp.symCSIndex, module->modulesSliceOffset + sizeof(SwagModule) + offsetof(SwagModule, types));
    for (auto& dep : module->moduleDependencies)
    {
        if (!dep->module->isSwag)
        {
            pp.emit_Add64_RCX(sizeof(SwagModule));
            continue;
        }

        auto callTable = dep->module->getGlobalPrivFct(g_LangSpec->name_getTypeTable);
        emitCall(pp, callTable);

        // Count types is stored as a uint64_t at the start of the address
        pp.emit_Load64_Indirect(0, R8, cc.returnByRegisterInteger);
        pp.emit_Store64_Indirect(sizeof(uint64_t), R8, RCX);
        pp.emit_Add64_Immediate(sizeof(uint64_t), cc.returnByRegisterInteger);

        pp.emit_Store64_Indirect(0, cc.returnByRegisterInteger, RCX);

        pp.emit_Add64_RCX(sizeof(SwagModule));
    }

    // Call to #init functions
    for (auto bc : module->byteCodeInitFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        emitCall(pp, bc->getCallName());
    }

    pp.emit_Add32_RSP(48);
    pp.emit_Pop(RDI);
    pp.emit_Ret();

    uint32_t endAddress = concat.totalCount();
    registerFunction(coffFct, startAddress, endAddress, sizeProlog, unwind);
    return true;
}

bool BackendX64::emitGlobalDrop(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    concat.align(16);
    auto startAddress = concat.totalCount();

    auto thisDrop        = module->getGlobalPrivFct(g_LangSpec->name_globalDrop);
    auto symbolFuncIndex = pp.getOrAddSymbol(thisDrop, CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    auto coffFct         = registerFunction(pp, nullptr, symbolFuncIndex);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::DynamicLib)
        pp.directives += Fmt("/EXPORT:%s ", thisDrop.c_str());

    auto beforeProlog = concat.totalCount();
    pp.emit_Sub32_RSP(40);
    auto                   sizeProlog = concat.totalCount() - beforeProlog;
    VectorNative<uint16_t> unwind;
    computeUnwind({}, {}, 40, sizeProlog, unwind);

    // Call to #drop functions
    for (auto bc : module->byteCodeDropFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        emitCall(pp, bc->getCallName());
    }

    // __dropGlobalVariables
    emitInternalCallExt(pp, module, g_LangSpec->name__dropGlobalVariables, pp.pushParams);

    pp.emit_Add32_RSP(40);
    pp.emit_Ret();

    uint32_t endAddress = concat.totalCount();
    registerFunction(coffFct, startAddress, endAddress, sizeProlog, unwind);
    return true;
}