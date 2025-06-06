#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/Context.h"
#include "Backend/SCBE/Main/Scbe.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

void Scbe::emitOS(ScbeCpu& pp)
{
    auto& concat = pp.concat;

    if (g_CommandLine.target.os == SwagTargetOs::Windows)
    {
        SWAG_ASSERT(g_CommandLine.target.arch == SwagTargetArch::X86_64);

        // @ChkStk Stack probing
        // See SWAG_LIMIT_PAGE_STACK
        pp.addFunction(R"(__chkstk)", CallConv::get(CallConvKind::X86_64), nullptr);
        pp.startFunction();
        concat.addString1("\x51");                            // push rcx  // NOLINT(modernize-raw-string-literal)
        concat.addString1("\x50");                            // push rax  // NOLINT(modernize-raw-string-literal)
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
        concat.addString1("\x58");                            // pop rax  // NOLINT(modernize-raw-string-literal)
        concat.addString1("\x59");                            // pop rcx  // NOLINT(modernize-raw-string-literal)
        concat.addString1("\xc3");                            // ret
        pp.cpuFct->endAddress = concat.totalCount();

        // int _DllMainCRTStartup(void*, int, void*)
        pp.getOrAddSymbol("_DllMainCRTStartup", CpuSymbolKind::Function, concat.totalCount() - pp.textSectionOffset);
        pp.emitLoadRegImm(CallConv::get(CallConvKind::X86_64)->returnByRegisterInteger, 1, OpBits::B64);
        pp.emitRet();
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void Scbe::emitMain(ScbeCpu& pp)
{
    const auto& buildParameters = pp.buildParams;
    const auto  module          = pp.module;

    const char*     entryPoint = nullptr;
    const CallConv* cc         = nullptr;
    switch (g_CommandLine.target.os)
    {
        case SwagTargetOs::Windows:
            if (buildParameters.buildCfg->backendSubKind == BuildCfgBackendSubKind::Console)
                entryPoint = "mainCRTStartup";
            else
                entryPoint = "WinMainCRTStartup";
            cc = CallConv::get(CallConvKind::X86_64);
            break;
        default:
            SWAG_ASSERT(false);
            return;
    }

    pp.addFunction(entryPoint, cc, nullptr);
    pp.startFunction();
    pp.cpuFct->unwindRegs.push_back(cc->nonVolatileRegistersInteger[0]);
    pp.emitEnter(0);

    // Set default system allocator function
    SWAG_ASSERT(g_SystemAllocatorTable);
    const auto bcAlloc = static_cast<ByteCode*>(ByteCode::undoByteCodeLambda(static_cast<void**>(g_SystemAllocatorTable)[0]));
    SWAG_ASSERT(bcAlloc);
    const auto sym = pp.getOrAddSymbol(bcAlloc->getCallName(), CpuSymbolKind::Extern);

    pp.emitLoadSymRelocAddress(cc->computeRegI0, pp.symDefaultAllocTable, 0);
    pp.emitLoadSymRelocAddress(cc->computeRegI1, sym->index, 0);
    pp.emitLoadMemReg(cc->computeRegI0, 0, cc->computeRegI1, OpBits::B64);

    // mainContext.allocator.itable = &defaultAllocTable;
    pp.emitLoadSymRelocAddress(cc->computeRegI1, pp.symMC_mainContext_allocator_itable, 0);
    pp.emitLoadMemReg(cc->computeRegI1, 0, cc->computeRegI0, OpBits::B64);

    // main context flags
    pp.emitLoadSymRelocAddress(cc->computeRegI1, pp.symMC_mainContext_flags, 0);
    const uint64_t contextFlags = getDefaultContextFlags(module);
    pp.emitLoadMemImm(cc->computeRegI1, 0, contextFlags, OpBits::B64);

    //__process_infos.contextTlsId = swag_runtime_tlsAlloc();
    pp.emitLoadSymRelocAddress(cc->nonVolatileRegistersInteger[0], pp.symPI_contextTlsId, 0);
    emitInternalCallRAParams(pp, g_LangSpec->name_priv_tlsAlloc, {}, cc->nonVolatileRegistersInteger[0], 0);

    //__process_infos.modules
    pp.emitLoadSymRelocAddress(cc->computeRegI1, pp.symPI_modulesAddr, 0);
    pp.emitLoadSymRelocAddress(cc->computeRegI0, pp.symCSIndex, module->modulesSliceOffset);
    pp.emitLoadMemReg(cc->computeRegI1, 0, cc->computeRegI0, OpBits::B64);
    pp.emitLoadSymRelocAddress(cc->computeRegI0, pp.symPI_modulesCount, 0);
    pp.emitLoadMemImm(cc->computeRegI0, 0, module->moduleDependencies.count + 1, OpBits::B64);

    //__process_infos.args
    pp.emitClearReg(cc->computeRegI1, OpBits::B64);
    pp.emitLoadSymRelocAddress(cc->computeRegI0, pp.symPI_argsAddr, 0);
    pp.emitLoadMemReg(cc->computeRegI0, 0, cc->computeRegI1, OpBits::B64);
    pp.emitLoadSymRelocAddress(cc->computeRegI0, pp.symPI_argsCount, 0);
    pp.emitLoadMemReg(cc->computeRegI0, 0, cc->computeRegI1, OpBits::B64);

    // Set main context
    pp.emitLoadSymRelocAddress(cc->computeRegI0, pp.symMC_mainContext, 0);
    pp.emitLoadSymRelocAddress(cc->computeRegI1, pp.symPI_defaultContext, 0);
    pp.emitLoadMemReg(cc->computeRegI1, 0, cc->computeRegI0, OpBits::B64);

    // Set current backend as SCBE
    pp.emitLoadSymRelocAddress(cc->computeRegI1, pp.symPI_backendKind, 0);
    pp.emitLoadMemImm(cc->computeRegI1, 0, static_cast<uint32_t>(SwagBackendGenType::SCBE), OpBits::B32);

    // Set default context in TLS
    pp.pushParams.clear();
    pp.pushParams.push_back({.type = CpuPushParamType::SymbolRelocValue, .value = pp.symPI_contextTlsId});
    pp.pushParams.push_back({.type = CpuPushParamType::SymbolRelocValue, .value = pp.symPI_defaultContext});
    emitInternalCallCPUParams(pp, g_LangSpec->name_priv_tlsSetValue, pp.pushParams);

    // Setup runtime
    const auto rtFlags = getRuntimeFlags();
    pp.pushParams.clear();
    pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = rtFlags, .typeInfo = g_TypeMgr->typeInfoU64});
    emitInternalCallCPUParams(pp, g_LangSpec->name_priv_setupRuntime, pp.pushParams);

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
            pp.pushParams.push_back({.type = CpuPushParamType::GlobalString, .value = reinterpret_cast<uint64_t>(nameLib.cstr())});
            pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = nameLib.length()});
            emitInternalCallCPUParams(pp, g_LangSpec->name_priv_loaddll, pp.pushParams);
        }
    }

    pp.pushParams.clear();
    pp.pushParams.push_back({.type = CpuPushParamType::SymbolRelocAddr, .value = pp.symPI_processInfos});

    // Call to global init of all dependencies
    for (const auto dep : moduleDependencies)
    {
        SWAG_ASSERT(dep->module);
        if (!dep->module->isSwag)
            continue;
        auto nameFct = dep->module->getGlobalPrivateFct(g_LangSpec->name_globalInit);
        emitInternalCallCPUParams(pp, nameFct, pp.pushParams);
    }

    // Call to global init of this module
    auto thisInit = module->getGlobalPrivateFct(g_LangSpec->name_globalInit);
    emitInternalCallCPUParams(pp, thisInit, pp.pushParams);

    // Call to global premain of all dependencies
    for (const auto dep : moduleDependencies)
    {
        SWAG_ASSERT(dep->module);
        if (!dep->module->isSwag)
            continue;

        auto nameFct = dep->module->getGlobalPrivateFct(g_LangSpec->name_globalPreMain);
        emitInternalCallCPUParams(pp, nameFct, pp.pushParams);
    }

    // Call to global premain of this module
    thisInit = module->getGlobalPrivateFct(g_LangSpec->name_globalPreMain);
    emitInternalCallCPUParams(pp, thisInit, pp.pushParams);

    // Call to test functions
    if (buildParameters.compileType == Test)
    {
        for (const auto bc : module->byteCodeTestFunc)
        {
            const auto node = bc->node;
            if (node && node->hasAttribute(ATTRIBUTE_COMPILER))
                continue;

            pp.emitCallLocal(bc->getCallName(), CallConv::get(CallConvKind::Swag));
        }
    }

    // Call to main
    if (module->byteCodeMainFunc)
    {
        pp.emitCallLocal(module->byteCodeMainFunc->getCallName(), CallConv::get(CallConvKind::Swag));
    }

    // Call to global drop of this module
    const auto thisDrop = module->getGlobalPrivateFct(g_LangSpec->name_globalDrop);
    pp.emitCallLocal(thisDrop, CallConv::get(CallConvKind::Swag));

    // Call to global drop of all dependencies
    for (uint32_t i = moduleDependencies.size() - 1; i != UINT32_MAX; i--)
    {
        const auto dep = moduleDependencies[i];
        if (!dep->module->isSwag)
            continue;
        auto nameFct = dep->module->getGlobalPrivateFct(g_LangSpec->name_globalDrop);
        pp.emitCallLocal(nameFct, CallConv::get(CallConvKind::Swag));
    }

    pp.emitCallLocal(g_LangSpec->name_priv_closeRuntime, CallConv::get(CallConvKind::Swag));
    pp.emitClearReg(cc->returnByRegisterInteger, OpBits::B64);

    pp.emitLeave();
    pp.endFunction();
}

void Scbe::emitGetTypeTable(ScbeCpu& pp)
{
    const auto& buildParameters = pp.buildParams;
    const auto  module          = pp.module;
    if (buildParameters.buildCfg->backendKind != BuildCfgBackendKind::Library)
        return;

    const auto cc       = g_TypeMgr->typeInfoModuleCall->getCallConv();
    const auto thisInit = module->getGlobalPrivateFct(g_LangSpec->name_getTypeTable);
    pp.addFunction(thisInit, cc, nullptr);
    pp.startFunction();
    pp.emitEnter(0);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
        pp.directives += form("/EXPORT:%s ", thisInit.cstr());

    pp.emitLoadSymRelocAddress(cc->returnByRegisterInteger, pp.symCSIndex, module->typesSliceOffset);

    pp.emitLeave();
    pp.endFunction();
}

void Scbe::emitGlobalPreMain(ScbeCpu& pp)
{
    const auto& buildParameters = pp.buildParams;
    const auto  module          = pp.module;

    const auto& cc       = g_TypeMgr->typeInfoModuleCall->getCallConv();
    const auto  thisInit = module->getGlobalPrivateFct(g_LangSpec->name_globalPreMain);
    pp.addFunction(thisInit, cc, nullptr);
    pp.startFunction();

    pp.emitEnter(0);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
        pp.directives += form("/EXPORT:%s ", thisInit.cstr());

    // Store first parameter on stack (process infos ptr)
    SWAG_ASSERT(!cc->paramsRegistersInteger.empty());
    pp.emitLoadMemReg(CpuReg::Rsp, 0, cc->paramsRegistersInteger[0], OpBits::B64);

    // Copy process infos passed as a parameter to the process info struct of this module
    pp.pushParams.clear();
    pp.pushParams.push_back({.type = CpuPushParamType::SymbolRelocAddr, .value = pp.symPI_processInfos});
    pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = 0});
    pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = sizeof(SwagProcessInfos)});
    emitInternalCallCPUParams(pp, g_LangSpec->name_memcpy, pp.pushParams);

    // Call to #premain functions
    for (const auto bc : module->byteCodePreMainFunc)
    {
        const auto node = bc->node;
        if (node && node->hasAttribute(ATTRIBUTE_COMPILER))
            continue;
        pp.emitCallLocal(bc->getCallName(), CallConv::get(CallConvKind::Swag));
    }

    pp.emitLeave();
    pp.endFunction();
}

void Scbe::emitGlobalInit(ScbeCpu& pp)
{
    const auto& buildParameters = pp.buildParams;
    const auto  module          = pp.module;

    const auto thisInit = module->getGlobalPrivateFct(g_LangSpec->name_globalInit);
    const auto cc       = g_TypeMgr->typeInfoModuleCall->getCallConv();
    pp.addFunction(thisInit, cc, nullptr);
    pp.startFunction();

    pp.cpuFct->unwindRegs.push_back(cc->nonVolatileRegistersInteger[0]);
    pp.emitEnter(0);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
        pp.directives += form("/EXPORT:%s ", thisInit.cstr());

    // Store first parameter on stack (process infos ptr)
    SWAG_ASSERT(cc->paramsRegistersInteger.size() >= 1);
    pp.emitLoadMemReg(CpuReg::Rsp, 0, cc->paramsRegistersInteger[0], OpBits::B64);

    // Copy process infos passed as a parameter to the process info struct of this module
    pp.pushParams.clear();
    pp.pushParams.push_back({.type = CpuPushParamType::SymbolRelocAddr, .value = pp.symPI_processInfos});
    pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = 0});
    pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = sizeof(SwagProcessInfos)});
    emitInternalCallCPUParams(pp, g_LangSpec->name_memcpy, pp.pushParams);

    // Thread local storage
    pp.pushParams.clear();
    pp.emitLoadSymRelocAddress(cc->nonVolatileRegistersInteger[0], pp.symTls_threadLocalId, 0);
    emitInternalCallCPUParams(pp, g_LangSpec->name_priv_tlsAlloc, pp.pushParams, cc->nonVolatileRegistersInteger[0], 0);

    // Init type table slice for each dependency (by calling ???_getTypeTable)
    const auto resReg = CallConv::getVolatileRegisterInteger(cc, cc, VF_EXCLUDE_COMPUTE | VF_EXCLUDE_PARAM0 | VF_EXCLUDE_RETURN);
    pp.emitLoadSymRelocAddress(resReg, pp.symCSIndex, module->modulesSliceOffset + sizeof(SwagModule) + offsetof(SwagModule, types));
    for (const auto& dep : module->moduleDependencies)
    {
        if (dep->module->isSwag)
        {
            pp.pushParams.clear();
            pp.pushParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = resReg, .value = 0});
            auto callTable = dep->module->getGlobalPrivateFct(g_LangSpec->name_getTypeTable);
            emitInternalCallCPUParams(pp, callTable, pp.pushParams);

            // Count types is stored as a uint64_t at the start of the address
            pp.emitLoadRegMem(cc->nonVolatileRegistersInteger[0], cc->returnByRegisterInteger, 0, OpBits::B64);
            pp.emitLoadMemReg(resReg, sizeof(uint64_t), cc->nonVolatileRegistersInteger[0], OpBits::B64);
            pp.emitOpBinaryRegImm(pp.cpuFct->cc->returnByRegisterInteger, sizeof(uint64_t), CpuOp::ADD, OpBits::B64);
            pp.emitLoadMemReg(resReg, 0, cc->returnByRegisterInteger, OpBits::B64);
        }

        pp.emitOpBinaryRegImm(resReg, sizeof(SwagModule), CpuOp::ADD, OpBits::B64);
    }

    // Call to #init functions
    for (const auto bc : module->byteCodeInitFunc)
    {
        const auto node = bc->node;
        if (node && node->hasAttribute(ATTRIBUTE_COMPILER))
            continue;
        pp.emitCallLocal(bc->getCallName(), CallConv::get(CallConvKind::Swag));
    }

    pp.emitLeave();
    pp.endFunction();
}

void Scbe::emitGlobalDrop(ScbeCpu& pp)
{
    const auto  module          = pp.module;
    const auto& buildParameters = pp.buildParams;

    const auto cc       = g_TypeMgr->typeInfoModuleCall->getCallConv();
    const auto thisDrop = module->getGlobalPrivateFct(g_LangSpec->name_globalDrop);
    pp.addFunction(thisDrop, cc, nullptr);
    pp.startFunction();
    pp.emitEnter(0);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
        pp.directives += form("/EXPORT:%s ", thisDrop.cstr());

    // Call to #drop functions
    for (const auto bc : module->byteCodeDropFunc)
    {
        const auto node = bc->node;
        if (node && node->hasAttribute(ATTRIBUTE_COMPILER))
            continue;
        pp.emitCallLocal(bc->getCallName(), CallConv::get(CallConvKind::Swag));
    }

    // __dropGlobalVariables
    pp.pushParams.clear();
    emitInternalCallCPUParams(pp, g_LangSpec->name_priv_dropGlobalVariables, pp.pushParams);

    pp.emitLeave();
    pp.endFunction();
}
