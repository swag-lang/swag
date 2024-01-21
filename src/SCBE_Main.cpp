#include "pch.h"
#include "SCBE.h"
#include "ByteCode.h"
#include "Context.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Report.h"
#include "TypeManager.h"

bool SCBE::emitOS(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    concat.align(16);
    if (g_CommandLine.target.os == SwagTargetOs::Windows)
    {
        // :ChkStk Stack probing
        // See SWAG_LIMIT_PAGE_STACK
        auto symbolFuncIndex = pp.getOrAddSymbol("__chkstk", CPUSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
        auto cpuFct          = pp.registerFunction(nullptr, symbolFuncIndex);
        SWAG_ASSERT(g_CommandLine.target.arch == SwagTargetArch::X86_64);
        cpuFct->startAddress = concat.totalCount();
        if (g_CommandLine.target.arch == SwagTargetArch::X86_64)
        {
            concat.addString1("\x51");                           // push rcx
            concat.addString1("\x50");                           // push rax
            concat.addString("\x48\x3d\x00\x10\x00\x00", 6);     // cmp rax, 1000h
            concat.addString5("\x48\x8d\x4c\x24\x18");           // lea rcx[rsp+18h]
            concat.addString2("\x72\x18");                       // jb @1
            concat.addString("\x48\x81\xe9\x00\x10\x00\x00", 7); // @2 sub rcx, 1000h
            concat.addString3("\x48\x85\x09");                   // test qword ptr [rcx], rax
            concat.addString("\x48\x2d\x00\x10\x00\x00", 6);     // sub rax, 1000h
            concat.addString("\x48\x3d\x00\x10\x00\x00", 6);     // cmp rax, 1000h
            concat.addString2("\x77\xe8");                       // ja @2
            concat.addString3("\x48\x29\xc1");                   // @1 sub rcx, rax
            concat.addString3("\x48\x85\x09");                   // test qword ptr [rcx], rcx
            concat.addString1("\x58");                           // pop rax
            concat.addString1("\x59");                           // pop rcx
            concat.addString1("\xc3");                           // ret
        }
        cpuFct->endAddress = concat.totalCount();

        // int _DllMainCRTStartup(void*, int, void*)
        pp.getOrAddSymbol("_DllMainCRTStartup", CPUSymbolKind::Function, concat.totalCount() - pp.textSectionOffset);
        pp.emit_Load64_Immediate(RAX, 1);
        pp.emit_Ret();
        return true;
    }
    else
    {
        Report::error(module, Fmt(Err(Err0735), Backend::getOsName(g_CommandLine.target)));
        return false;
    }
}

bool SCBE::emitMain(const BuildParameters& buildParameters)
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
        if (buildParameters.buildCfg->backendSubKind == BuildCfgBackendSubKind::Console)
            entryPoint = "mainCRTStartup";
        else
            entryPoint = "WinMainCRTStartup";
        break;
    default:
        Report::error(module, Fmt(Err(Err0735), Backend::getOsName(g_CommandLine.target)));
        return false;
    }

    auto symbolFuncIndex = pp.getOrAddSymbol(entryPoint, CPUSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    auto cpuFct          = pp.registerFunction(nullptr, symbolFuncIndex);

    VectorNative<uint16_t> unwind;
    auto                   beforeProlog = concat.totalCount();
    pp.emit_OpN_Immediate(RSP, 40, CPUOp::SUB, CPUBits::B64);
    auto sizeProlog = concat.totalCount() - beforeProlog;
    SCBE::computeUnwind({}, {}, 40, sizeProlog, unwind);

    // Set default system allocator function
    SWAG_ASSERT(g_SystemAllocatorTable);
    auto bcAlloc = (ByteCode*) ByteCode::undoByteCodeLambda(((void**) g_SystemAllocatorTable)[0]);
    SWAG_ASSERT(bcAlloc);
    pp.emit_Symbol_RelocationAddr(RAX, pp.symDefaultAllocTable, 0);
    pp.emit_LoadAddress_Indirect(0, RCX, RIP);
    pp.emit_Symbol_RelocationRef(bcAlloc->getCallName());
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
    pp.emit_ClearN(RCX, CPUBits::B64);
    pp.emit_Symbol_RelocationAddr(RAX, pp.symPI_argsAddr, 0);
    pp.emit_Store64_Indirect(0, RCX, RAX);
    pp.emit_Symbol_RelocationAddr(RAX, pp.symPI_argsCount, 0);
    pp.emit_Store64_Indirect(0, RCX, RAX);

    // Set main context
    pp.emit_Symbol_RelocationAddr(RAX, pp.symMC_mainContext, 0);
    pp.emit_Symbol_RelocationAddr(RCX, pp.symPI_defaultContext, 0);
    pp.emit_Store64_Indirect(0, RAX, RCX);

    // Set current backend as SCBE
    pp.emit_Symbol_RelocationAddr(RCX, pp.symPI_backendKind, 0);
    pp.emit_Store32_Immediate(0, (uint32_t) SwagBackendGenType::SCBE, RCX);

    // Set default context in TLS
    pp.pushParams.clear();
    pp.pushParams.push_back({CPUPushParamType::RelocV, pp.symPI_contextTlsId});
    pp.pushParams.push_back({CPUPushParamType::RelocV, pp.symPI_defaultContext});
    emitInternalCallExt(pp, module, g_LangSpec->name__tlsSetValue, pp.pushParams);

    // Setup runtime
    auto rtFlags = Backend::getRuntimeFlags(module);
    pp.pushParams.clear();
    pp.pushParams.push_back({CPUPushParamType::Imm64, rtFlags});
    emitInternalCallExt(pp, module, g_LangSpec->name__setupRuntime, pp.pushParams);

    // Load all dependencies
    VectorNative<ModuleDependency*> moduleDependencies;
    module->sortDependenciesByInitOrder(moduleDependencies);
    for (size_t i = 0; i < moduleDependencies.size(); i++)
    {
        auto dep      = moduleDependencies[i];
        auto nameDown = dep->name;
        Ast::normalizeIdentifierName(nameDown);
        auto nameLib = nameDown;
        nameLib += Backend::getOutputFileExtension(g_CommandLine.target, BuildCfgBackendKind::DynamicLib);

        pp.pushParams.clear();
        pp.pushParams.push_back({CPUPushParamType::GlobalString, (uint64_t) nameLib.c_str()});
        pp.pushParams.push_back({CPUPushParamType::Imm, (uint64_t) nameLib.length()});
        emitInternalCallExt(pp, module, g_LangSpec->name__loaddll, pp.pushParams);
    }

    pp.pushParams.clear();
    pp.pushParams.push_back({CPUPushParamType::RelocAddr, pp.symPI_processInfos});

    // Call to global init of all dependencies
    for (size_t i = 0; i < moduleDependencies.size(); i++)
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
    for (size_t i = 0; i < moduleDependencies.size(); i++)
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
    if (buildParameters.compileType == BackendCompileType::Test)
    {
        for (auto bc : module->byteCodeTestFunc)
        {
            auto node = bc->node;
            if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;

            pp.emit_Call(bc->getCallName());
        }
    }

    // Call to main
    if (module->byteCodeMainFunc)
    {
        pp.emit_Call(module->byteCodeMainFunc->getCallName());
    }

    // Call to global drop of this module
    auto thisDrop = module->getGlobalPrivFct(g_LangSpec->name_globalDrop);
    pp.emit_Call(thisDrop);

    // Call to global drop of all dependencies
    for (int i = (int) moduleDependencies.size() - 1; i >= 0; i--)
    {
        auto dep = moduleDependencies[i];
        if (!dep->module->isSwag)
            continue;
        auto nameFct = dep->module->getGlobalPrivFct(g_LangSpec->name_globalDrop);
        pp.emit_Call(nameFct);
    }

    pp.emit_Call(g_LangSpec->name__closeRuntime);

    pp.emit_ClearN(RAX, CPUBits::B64);
    pp.emit_OpN_Immediate(RSP, 40, CPUOp::ADD, CPUBits::B64);
    pp.emit_Ret();

    uint32_t endAddress = concat.totalCount();
    initFunction(cpuFct, startAddress, endAddress, sizeProlog, unwind);
    return true;
}

bool SCBE::emitGetTypeTable(const BuildParameters& buildParameters)
{
    if (buildParameters.buildCfg->backendKind != BuildCfgBackendKind::DynamicLib)
        return true;

    int         ct              = buildParameters.compileType;
    int         precompileIndex = buildParameters.precompileIndex;
    auto&       pp              = *perThread[ct][precompileIndex];
    auto&       concat          = pp.concat;
    const auto& cc              = g_TypeMgr->typeInfoModuleCall->getCallConv();

    concat.align(16);
    auto startAddress = concat.totalCount();

    auto thisInit        = module->getGlobalPrivFct(g_LangSpec->name_getTypeTable);
    auto symbolFuncIndex = pp.getOrAddSymbol(thisInit, CPUSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    auto cpuFct          = pp.registerFunction(nullptr, symbolFuncIndex);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::DynamicLib)
        pp.directives += Fmt("/EXPORT:%s ", thisInit.c_str());

    VectorNative<uint16_t> unwind;
    auto                   beforeProlog = concat.totalCount();
    pp.emit_OpN_Immediate(RSP, 40, CPUOp::SUB, CPUBits::B64);
    auto sizeProlog = concat.totalCount() - beforeProlog;
    SCBE::computeUnwind({}, {}, 40, sizeProlog, unwind);

    pp.emit_OpN_Immediate(RSP, 40, CPUOp::ADD, CPUBits::B64);
    pp.emit_Symbol_RelocationAddr(cc.returnByRegisterInteger, pp.symCSIndex, module->typesSliceOffset);
    pp.emit_Ret();

    uint32_t endAddress = concat.totalCount();
    initFunction(cpuFct, startAddress, endAddress, sizeProlog, unwind);

    return true;
}

bool SCBE::emitGlobalPreMain(const BuildParameters& buildParameters)
{
    int         ct              = buildParameters.compileType;
    int         precompileIndex = buildParameters.precompileIndex;
    auto&       pp              = *perThread[ct][precompileIndex];
    auto&       concat          = pp.concat;
    const auto& cc              = g_TypeMgr->typeInfoModuleCall->getCallConv();

    concat.align(16);
    auto startAddress = concat.totalCount();

    auto thisInit        = module->getGlobalPrivFct(g_LangSpec->name_globalPreMain);
    auto symbolFuncIndex = pp.getOrAddSymbol(thisInit, CPUSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    auto cpuFct          = pp.registerFunction(nullptr, symbolFuncIndex);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::DynamicLib)
        pp.directives += Fmt("/EXPORT:%s ", thisInit.c_str());

    VectorNative<uint16_t> unwind;
    auto                   beforeProlog = concat.totalCount();
    pp.emit_Push(RDI);
    pp.emit_OpN_Immediate(RSP, 48, CPUOp::SUB, CPUBits::B64);
    auto sizeProlog = concat.totalCount() - beforeProlog;
    SCBE::computeUnwind({}, {}, 48, sizeProlog, unwind);

    // Store first parameter on stack (process infos ptr)
    SWAG_ASSERT(cc.paramByRegisterCount >= 1);
    pp.emit_LoadAddress_Indirect(0, RDI, RSP);
    pp.emit_Store64_Indirect(0, cc.paramByRegisterInteger[0], RDI);

    // Copy process infos passed as a parameter to the process info struct of this module
    pp.pushParams.clear();
    pp.pushParams.push_back({CPUPushParamType::RelocAddr, pp.symPI_processInfos});
    pp.pushParams.push_back({CPUPushParamType::Reg, 0});
    pp.pushParams.push_back({CPUPushParamType::Imm, sizeof(SwagProcessInfos)});
    emitInternalCallExt(pp, module, g_LangSpec->name_memcpy, pp.pushParams);

    // Call to #premain functions
    for (auto bc : module->byteCodePreMainFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        pp.emit_Call(bc->getCallName());
    }

    pp.emit_OpN_Immediate(RSP, 48, CPUOp::ADD, CPUBits::B64);
    pp.emit_Pop(RDI);
    pp.emit_Ret();

    uint32_t endAddress = concat.totalCount();
    initFunction(cpuFct, startAddress, endAddress, sizeProlog, unwind);
    return true;
}

bool SCBE::emitGlobalInit(const BuildParameters& buildParameters)
{
    int         ct              = buildParameters.compileType;
    int         precompileIndex = buildParameters.precompileIndex;
    auto&       pp              = *perThread[ct][precompileIndex];
    auto&       concat          = pp.concat;
    const auto& cc              = g_TypeMgr->typeInfoModuleCall->getCallConv();

    concat.align(16);
    auto startAddress = concat.totalCount();

    auto thisInit        = module->getGlobalPrivFct(g_LangSpec->name_globalInit);
    auto symbolFuncIndex = pp.getOrAddSymbol(thisInit, CPUSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    auto cpuFct          = pp.registerFunction(nullptr, symbolFuncIndex);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::DynamicLib)
        pp.directives += Fmt("/EXPORT:%s ", thisInit.c_str());

    VectorNative<uint16_t> unwind;
    auto                   beforeProlog = concat.totalCount();
    pp.emit_Push(RDI);
    pp.emit_OpN_Immediate(RSP, 48, CPUOp::SUB, CPUBits::B64);
    auto sizeProlog = concat.totalCount() - beforeProlog;
    SCBE::computeUnwind({}, {}, 48, sizeProlog, unwind);

    // Store first parameter on stack (process infos ptr)
    SWAG_ASSERT(cc.paramByRegisterCount >= 1);
    pp.emit_LoadAddress_Indirect(0, RDI, RSP);
    pp.emit_Store64_Indirect(0, cc.paramByRegisterInteger[0], RDI);

    // Copy process infos passed as a parameter to the process info struct of this module
    pp.pushParams.clear();
    pp.pushParams.push_back({CPUPushParamType::RelocAddr, pp.symPI_processInfos});
    pp.pushParams.push_back({CPUPushParamType::Reg, 0});
    pp.pushParams.push_back({CPUPushParamType::Imm, sizeof(SwagProcessInfos)});
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
            pp.emit_OpN_Immediate(RCX, sizeof(SwagModule), CPUOp::ADD, CPUBits::B64);
            continue;
        }

        auto callTable = dep->module->getGlobalPrivFct(g_LangSpec->name_getTypeTable);
        pp.emit_Call(callTable);

        // Count types is stored as a uint64_t at the start of the address
        pp.emit_Load64_Indirect(0, R8, cc.returnByRegisterInteger);
        pp.emit_Store64_Indirect(sizeof(uint64_t), R8, RCX);
        pp.emit_OpN_Immediate(cc.returnByRegisterInteger, sizeof(uint64_t), CPUOp::ADD, CPUBits::B64);
        pp.emit_Store64_Indirect(0, cc.returnByRegisterInteger, RCX);

        pp.emit_OpN_Immediate(RCX, sizeof(SwagModule), CPUOp::ADD, CPUBits::B64);
    }

    // Call to #init functions
    for (auto bc : module->byteCodeInitFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        pp.emit_Call(bc->getCallName());
    }

    pp.emit_OpN_Immediate(RSP, 48, CPUOp::ADD, CPUBits::B64);
    pp.emit_Pop(RDI);
    pp.emit_Ret();

    uint32_t endAddress = concat.totalCount();
    initFunction(cpuFct, startAddress, endAddress, sizeProlog, unwind);
    return true;
}

bool SCBE::emitGlobalDrop(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    concat.align(16);
    auto startAddress = concat.totalCount();

    auto thisDrop        = module->getGlobalPrivFct(g_LangSpec->name_globalDrop);
    auto symbolFuncIndex = pp.getOrAddSymbol(thisDrop, CPUSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    auto cpuFct          = pp.registerFunction(nullptr, symbolFuncIndex);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::DynamicLib)
        pp.directives += Fmt("/EXPORT:%s ", thisDrop.c_str());

    VectorNative<uint16_t> unwind;
    auto                   beforeProlog = concat.totalCount();
    pp.emit_OpN_Immediate(RSP, 40, CPUOp::SUB, CPUBits::B64);
    auto sizeProlog = concat.totalCount() - beforeProlog;
    SCBE::computeUnwind({}, {}, 40, sizeProlog, unwind);

    // Call to #drop functions
    for (auto bc : module->byteCodeDropFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        pp.emit_Call(bc->getCallName());
    }

    // __dropGlobalVariables
    emitInternalCallExt(pp, module, g_LangSpec->name__dropGlobalVariables, pp.pushParams);

    pp.emit_OpN_Immediate(RSP, 40, CPUOp::ADD, CPUBits::B64);
    pp.emit_Ret();

    uint32_t endAddress = concat.totalCount();
    initFunction(cpuFct, startAddress, endAddress, sizeProlog, unwind);
    return true;
}