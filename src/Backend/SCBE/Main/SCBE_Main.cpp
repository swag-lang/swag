#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/Context.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"
#include "Backend/SCBE/Main/SCBE.h"
#include "Semantic/Type/TypeManager.h"

void SCBE::emitOS(const BuildParameters& buildParameters) const
{
    const auto ct              = buildParameters.compileType;
    const auto precompileIndex = buildParameters.precompileIndex;
    auto&      pp              = encoder<SCBE_X64>(ct, precompileIndex);
    auto&      concat          = pp.concat;

    concat.align(16);
    if (g_CommandLine.target.os == SwagTargetOs::Windows)
    {
        // :ChkStk Stack probing
        // See SWAG_LIMIT_PAGE_STACK
        const auto symbolFuncIndex = pp.getOrAddSymbol("__chkstk", CPUSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
        const auto cpuFct          = pp.registerFunction(nullptr, symbolFuncIndex);
        SWAG_ASSERT(g_CommandLine.target.arch == SwagTargetArch::X86_64);
        cpuFct->startAddress = concat.totalCount();
        if (g_CommandLine.target.arch == SwagTargetArch::X86_64)
        {
            concat.addString1("\x51");                            // push rcx
            concat.addString1("\x50");                            // push rax
            concat.addStringN("\x48\x3d\x00\x10\x00\x00", 6);     // cmp rax, 1000h
            concat.addString5("\x48\x8d\x4c\x24\x18");            // lea rcx[rsp+18h]
            concat.addString2("\x72\x18");                        // jb @1
            concat.addStringN("\x48\x81\xe9\x00\x10\x00\x00", 7); // @2 sub rcx, 1000h
            concat.addString3("\x48\x85\x09");                    // test qword ptr [rcx], rax
            concat.addStringN("\x48\x2d\x00\x10\x00\x00", 6);     // sub rax, 1000h
            concat.addStringN("\x48\x3d\x00\x10\x00\x00", 6);     // cmp rax, 1000h
            concat.addString2("\x77\xe8");                        // ja @2
            concat.addString3("\x48\x29\xc1");                    // @1 sub rcx, rax
            concat.addString3("\x48\x85\x09");                    // test qword ptr [rcx], rcx
            concat.addString1("\x58");                            // pop rax
            concat.addString1("\x59");                            // pop rcx
            concat.addString1("\xc3");                            // ret
        }
        cpuFct->endAddress = concat.totalCount();

        // int _DllMainCRTStartup(void*, int, void*)
        pp.getOrAddSymbol("_DllMainCRTStartup", CPUSymbolKind::Function, concat.totalCount() - pp.textSectionOffset);
        pp.emitLoad64Immediate(RAX, 1);
        pp.emitRet();
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void SCBE::emitMain(const BuildParameters& buildParameters) const
{
    const auto ct              = buildParameters.compileType;
    const auto precompileIndex = buildParameters.precompileIndex;
    auto&      pp              = encoder<SCBE_X64>(ct, precompileIndex);
    auto&      concat          = pp.concat;

    concat.align(16);
    const auto startAddress = concat.totalCount();

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
            SWAG_ASSERT(false);
            return;
    }

    const auto symbolFuncIndex = pp.getOrAddSymbol(entryPoint, CPUSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    const auto cpuFct          = pp.registerFunction(nullptr, symbolFuncIndex);

    VectorNative<uint16_t> unwind;
    const auto             beforeProlog = concat.totalCount();
    pp.emitOpNImmediate(RSP, 40, CPUOp::SUB, CPUBits::B64);
    const auto sizeProlog = concat.totalCount() - beforeProlog;
    computeUnwind({}, {}, 40, sizeProlog, unwind);

    // Set default system allocator function
    SWAG_ASSERT(g_SystemAllocatorTable);
    const auto bcAlloc = static_cast<ByteCode*>(ByteCode::undoByteCodeLambda(static_cast<void**>(g_SystemAllocatorTable)[0]));
    SWAG_ASSERT(bcAlloc);
    pp.emitSymbolRelocationAddr(RAX, pp.symDefaultAllocTable, 0);
    pp.emitLoadAddressIndirect(0, RCX, RIP);
    pp.emitSymbolRelocationRef(bcAlloc->getCallName());
    pp.emitStore64Indirect(0, RCX, RAX);

    // mainContext.allocator.itable = &defaultAllocTable;
    pp.emitSymbolRelocationAddr(RCX, pp.symMC_mainContext_allocator_itable, 0);
    pp.emitStore64Indirect(0, RAX, RCX);

    // main context flags
    pp.emitSymbolRelocationAddr(RCX, pp.symMC_mainContext_flags, 0);
    const uint64_t contextFlags = getDefaultContextFlags(module);
    pp.emitStore64Immediate(0, contextFlags, RCX);

    //__process_infos.contextTlsId = swag_runtime_tlsAlloc();
    pp.emitSymbolRelocationAddr(RDI, pp.symPI_contextTlsId, 0);
    emitInternalCall(pp, g_LangSpec->name_priv_tlsAlloc, {}, 0);

    //__process_infos.modules
    pp.emitSymbolRelocationAddr(RCX, pp.symPI_modulesAddr, 0);
    pp.emitSymbolRelocationAddr(RAX, pp.symCSIndex, module->modulesSliceOffset);
    pp.emitStore64Indirect(0, RAX, RCX);
    pp.emitSymbolRelocationAddr(RAX, pp.symPI_modulesCount, 0);
    pp.emitStore64Immediate(0, module->moduleDependencies.count + 1, RAX);

    //__process_infos.args
    pp.emitClearN(RCX, CPUBits::B64);
    pp.emitSymbolRelocationAddr(RAX, pp.symPI_argsAddr, 0);
    pp.emitStore64Indirect(0, RCX, RAX);
    pp.emitSymbolRelocationAddr(RAX, pp.symPI_argsCount, 0);
    pp.emitStore64Indirect(0, RCX, RAX);

    // Set main context
    pp.emitSymbolRelocationAddr(RAX, pp.symMC_mainContext, 0);
    pp.emitSymbolRelocationAddr(RCX, pp.symPI_defaultContext, 0);
    pp.emitStore64Indirect(0, RAX, RCX);

    // Set current backend as SCBE
    pp.emitSymbolRelocationAddr(RCX, pp.symPI_backendKind, 0);
    pp.emitStore32Immediate(0, static_cast<uint32_t>(SwagBackendGenType::SCBE), RCX);

    // Set default context in TLS
    pp.pushParams.clear();
    pp.pushParams.push_back({CPUPushParamType::RelocV, pp.symPI_contextTlsId});
    pp.pushParams.push_back({CPUPushParamType::RelocV, pp.symPI_defaultContext});
    emitInternalCallExt(pp, g_LangSpec->name_priv_tlsSetValue, pp.pushParams);

    // Setup runtime
    const auto rtFlags = getRuntimeFlags();
    pp.pushParams.clear();
    pp.pushParams.push_back({CPUPushParamType::Imm64, rtFlags});
    emitInternalCallExt(pp, g_LangSpec->name_priv_setupRuntime, pp.pushParams);

    // Load all dependencies
    VectorNative<ModuleDependency*> moduleDependencies;
    module->sortDependenciesByInitOrder(moduleDependencies);
    for (const auto dep : moduleDependencies)
    {
        auto nameLib = getOutputFileName(g_CommandLine.target, dep->module->name, BuildCfgOutputKind::DynamicLib);

        std::error_code err;
        if (std::filesystem::exists(nameLib, err))
        {
            nameLib = nameLib.filename();
            pp.pushParams.clear();
            pp.pushParams.push_back({CPUPushParamType::GlobalString, reinterpret_cast<uint64_t>(nameLib.c_str())});
            pp.pushParams.push_back({CPUPushParamType::Imm, nameLib.length()});
            emitInternalCallExt(pp, g_LangSpec->name_priv_loaddll, pp.pushParams);
        }
    }

    pp.pushParams.clear();
    pp.pushParams.push_back({CPUPushParamType::RelocAddr, pp.symPI_processInfos});

    // Call to global init of all dependencies
    for (const auto dep : moduleDependencies)
    {
        SWAG_ASSERT(dep->module);
        if (!dep->module->isSwag)
            continue;
        auto nameFct = dep->module->getGlobalPrivFct(g_LangSpec->name_globalInit);
        emitInternalCallExt(pp, nameFct, pp.pushParams, UINT32_MAX, g_TypeMgr->typeInfoModuleCall);
    }

    // Call to global init of this module
    auto thisInit = module->getGlobalPrivFct(g_LangSpec->name_globalInit);
    emitInternalCallExt(pp, thisInit, pp.pushParams, UINT32_MAX, g_TypeMgr->typeInfoModuleCall);

    // Call to global premain of all dependencies
    for (const auto dep : moduleDependencies)
    {
        SWAG_ASSERT(dep->module);
        if (!dep->module->isSwag)
            continue;

        auto nameFct = dep->module->getGlobalPrivFct(g_LangSpec->name_globalPreMain);
        emitInternalCallExt(pp, nameFct, pp.pushParams, UINT32_MAX, g_TypeMgr->typeInfoModuleCall);
    }

    // Call to global premain of this module
    thisInit = module->getGlobalPrivFct(g_LangSpec->name_globalPreMain);
    emitInternalCallExt(pp, thisInit, pp.pushParams, UINT32_MAX, g_TypeMgr->typeInfoModuleCall);

    // Call to test functions
    if (buildParameters.compileType == Test)
    {
        for (const auto bc : module->byteCodeTestFunc)
        {
            const auto node = bc->node;
            if (node && node->hasAttribute(ATTRIBUTE_COMPILER))
                continue;

            pp.emitCall(bc->getCallName());
        }
    }

    // Call to main
    if (module->byteCodeMainFunc)
    {
        pp.emitCall(module->byteCodeMainFunc->getCallName());
    }

    // Call to global drop of this module
    const auto thisDrop = module->getGlobalPrivFct(g_LangSpec->name_globalDrop);
    pp.emitCall(thisDrop);

    // Call to global drop of all dependencies
    for (uint32_t i = moduleDependencies.size() - 1; i != UINT32_MAX; i--)
    {
        const auto dep = moduleDependencies[i];
        if (!dep->module->isSwag)
            continue;
        auto nameFct = dep->module->getGlobalPrivFct(g_LangSpec->name_globalDrop);
        pp.emitCall(nameFct);
    }

    pp.emitCall(g_LangSpec->name_priv_closeRuntime);

    pp.emitClearN(RAX, CPUBits::B64);
    pp.emitOpNImmediate(RSP, 40, CPUOp::ADD, CPUBits::B64);
    pp.emitRet();

    const uint32_t endAddress = concat.totalCount();
    initFunction(cpuFct, startAddress, endAddress, sizeProlog, unwind);
}

void SCBE::emitGetTypeTable(const BuildParameters& buildParameters) const
{
    if (buildParameters.buildCfg->backendKind != BuildCfgBackendKind::Library)
        return;

    const auto  ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    auto&       pp              = encoder<SCBE_X64>(ct, precompileIndex);
    auto&       concat          = pp.concat;
    const auto& cc              = g_TypeMgr->typeInfoModuleCall->getCallConv();

    concat.align(16);
    const auto startAddress = concat.totalCount();

    const auto thisInit        = module->getGlobalPrivFct(g_LangSpec->name_getTypeTable);
    const auto symbolFuncIndex = pp.getOrAddSymbol(thisInit, CPUSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    const auto cpuFct          = pp.registerFunction(nullptr, symbolFuncIndex);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
        pp.directives += form("/EXPORT:%s ", thisInit.c_str());

    VectorNative<uint16_t> unwind;
    const auto             beforeProlog = concat.totalCount();
    pp.emitOpNImmediate(RSP, 40, CPUOp::SUB, CPUBits::B64);
    const auto sizeProlog = concat.totalCount() - beforeProlog;
    computeUnwind({}, {}, 40, sizeProlog, unwind);

    pp.emitOpNImmediate(RSP, 40, CPUOp::ADD, CPUBits::B64);
    pp.emitSymbolRelocationAddr(cc.returnByRegisterInteger, pp.symCSIndex, module->typesSliceOffset);
    pp.emitRet();

    const uint32_t endAddress = concat.totalCount();
    initFunction(cpuFct, startAddress, endAddress, sizeProlog, unwind);
}

void SCBE::emitGlobalPreMain(const BuildParameters& buildParameters) const
{
    const auto  ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    auto&       pp              = encoder<SCBE_X64>(ct, precompileIndex);
    auto&       concat          = pp.concat;
    const auto& cc              = g_TypeMgr->typeInfoModuleCall->getCallConv();

    concat.align(16);
    const auto startAddress = concat.totalCount();

    const auto thisInit        = module->getGlobalPrivFct(g_LangSpec->name_globalPreMain);
    const auto symbolFuncIndex = pp.getOrAddSymbol(thisInit, CPUSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    const auto cpuFct          = pp.registerFunction(nullptr, symbolFuncIndex);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
        pp.directives += form("/EXPORT:%s ", thisInit.c_str());

    VectorNative<uint16_t> unwind;
    const auto             beforeProlog = concat.totalCount();
    pp.emitPush(RDI);
    pp.emitOpNImmediate(RSP, 48, CPUOp::SUB, CPUBits::B64);
    const auto sizeProlog = concat.totalCount() - beforeProlog;
    computeUnwind({}, {}, 48, sizeProlog, unwind);

    // Store first parameter on stack (process infos ptr)
    SWAG_ASSERT(cc.paramByRegisterCount >= 1);
    pp.emitLoadAddressIndirect(0, RDI, RSP);
    pp.emitStore64Indirect(0, cc.paramByRegisterInteger[0], RDI);

    // Copy process infos passed as a parameter to the process info struct of this module
    pp.pushParams.clear();
    pp.pushParams.push_back({CPUPushParamType::RelocAddr, pp.symPI_processInfos});
    pp.pushParams.push_back({CPUPushParamType::Reg, 0});
    pp.pushParams.push_back({CPUPushParamType::Imm, sizeof(SwagProcessInfos)});
    emitInternalCallExt(pp, g_LangSpec->name_memcpy, pp.pushParams);

    // Call to #premain functions
    for (const auto bc : module->byteCodePreMainFunc)
    {
        const auto node = bc->node;
        if (node && node->hasAttribute(ATTRIBUTE_COMPILER))
            continue;
        pp.emitCall(bc->getCallName());
    }

    pp.emitOpNImmediate(RSP, 48, CPUOp::ADD, CPUBits::B64);
    pp.emitPop(RDI);
    pp.emitRet();

    const uint32_t endAddress = concat.totalCount();
    initFunction(cpuFct, startAddress, endAddress, sizeProlog, unwind);
}

void SCBE::emitGlobalInit(const BuildParameters& buildParameters) const
{
    const auto  ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    auto&       pp              = encoder<SCBE_X64>(ct, precompileIndex);
    auto&       concat          = pp.concat;
    const auto& cc              = g_TypeMgr->typeInfoModuleCall->getCallConv();

    concat.align(16);
    const auto startAddress = concat.totalCount();

    const auto thisInit        = module->getGlobalPrivFct(g_LangSpec->name_globalInit);
    const auto symbolFuncIndex = pp.getOrAddSymbol(thisInit, CPUSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    const auto cpuFct          = pp.registerFunction(nullptr, symbolFuncIndex);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
        pp.directives += form("/EXPORT:%s ", thisInit.c_str());

    VectorNative<uint16_t> unwind;
    const auto             beforeProlog = concat.totalCount();
    pp.emitPush(RDI);
    pp.emitOpNImmediate(RSP, 48, CPUOp::SUB, CPUBits::B64);
    const auto sizeProlog = concat.totalCount() - beforeProlog;
    computeUnwind({}, {}, 48, sizeProlog, unwind);

    // Store first parameter on stack (process infos ptr)
    SWAG_ASSERT(cc.paramByRegisterCount >= 1);
    pp.emitLoadAddressIndirect(0, RDI, RSP);
    pp.emitStore64Indirect(0, cc.paramByRegisterInteger[0], RDI);

    // Copy process infos passed as a parameter to the process info struct of this module
    pp.pushParams.clear();
    pp.pushParams.push_back({CPUPushParamType::RelocAddr, pp.symPI_processInfos});
    pp.pushParams.push_back({CPUPushParamType::Reg, 0});
    pp.pushParams.push_back({CPUPushParamType::Imm, sizeof(SwagProcessInfos)});
    emitInternalCallExt(pp, g_LangSpec->name_memcpy, pp.pushParams);

    // Thread local storage
    pp.emitSymbolRelocationAddr(RDI, pp.symTls_threadLocalId, 0);
    emitInternalCallExt(pp, g_LangSpec->name_priv_tlsAlloc, {}, 0);

    // Init type table slice for each dependency (by calling ???_getTypeTable)
    pp.emitSymbolRelocationAddr(RCX, pp.symCSIndex, module->modulesSliceOffset + sizeof(SwagModule) + offsetof(SwagModule, types));
    for (const auto& dep : module->moduleDependencies)
    {
        if (!dep->module->isSwag)
        {
            pp.emitOpNImmediate(RCX, sizeof(SwagModule), CPUOp::ADD, CPUBits::B64);
            continue;
        }

        auto callTable = dep->module->getGlobalPrivFct(g_LangSpec->name_getTypeTable);
        pp.emitCall(callTable);

        // Count types is stored as a uint64_t at the start of the address
        pp.emitLoad64Indirect(0, R8, cc.returnByRegisterInteger);
        pp.emitStore64Indirect(sizeof(uint64_t), R8, RCX);
        pp.emitOpNImmediate(cc.returnByRegisterInteger, sizeof(uint64_t), CPUOp::ADD, CPUBits::B64);
        pp.emitStore64Indirect(0, cc.returnByRegisterInteger, RCX);

        pp.emitOpNImmediate(RCX, sizeof(SwagModule), CPUOp::ADD, CPUBits::B64);
    }

    // Call to #init functions
    for (const auto bc : module->byteCodeInitFunc)
    {
        const auto node = bc->node;
        if (node && node->hasAttribute(ATTRIBUTE_COMPILER))
            continue;
        pp.emitCall(bc->getCallName());
    }

    pp.emitOpNImmediate(RSP, 48, CPUOp::ADD, CPUBits::B64);
    pp.emitPop(RDI);
    pp.emitRet();

    const uint32_t endAddress = concat.totalCount();
    initFunction(cpuFct, startAddress, endAddress, sizeProlog, unwind);
}

void SCBE::emitGlobalDrop(const BuildParameters& buildParameters) const
{
    const auto ct              = buildParameters.compileType;
    const auto precompileIndex = buildParameters.precompileIndex;
    auto&      pp              = encoder<SCBE_X64>(ct, precompileIndex);
    auto&      concat          = pp.concat;

    concat.align(16);
    const auto startAddress = concat.totalCount();

    const auto thisDrop        = module->getGlobalPrivFct(g_LangSpec->name_globalDrop);
    const auto symbolFuncIndex = pp.getOrAddSymbol(thisDrop, CPUSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    const auto cpuFct          = pp.registerFunction(nullptr, symbolFuncIndex);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
        pp.directives += form("/EXPORT:%s ", thisDrop.c_str());

    VectorNative<uint16_t> unwind;
    const auto             beforeProlog = concat.totalCount();
    pp.emitOpNImmediate(RSP, 40, CPUOp::SUB, CPUBits::B64);
    const auto sizeProlog = concat.totalCount() - beforeProlog;
    computeUnwind({}, {}, 40, sizeProlog, unwind);

    // Call to #drop functions
    for (const auto bc : module->byteCodeDropFunc)
    {
        const auto node = bc->node;
        if (node && node->hasAttribute(ATTRIBUTE_COMPILER))
            continue;
        pp.emitCall(bc->getCallName());
    }

    // __dropGlobalVariables
    emitInternalCallExt(pp, g_LangSpec->name_priv_dropGlobalVariables, pp.pushParams);

    pp.emitOpNImmediate(RSP, 40, CPUOp::ADD, CPUBits::B64);
    pp.emitRet();

    const uint32_t endAddress = concat.totalCount();
    initFunction(cpuFct, startAddress, endAddress, sizeProlog, unwind);
}
