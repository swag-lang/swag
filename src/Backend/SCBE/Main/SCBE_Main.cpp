#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/Context.h"
#include "Backend/SCBE/Main/SCBE.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

void SCBE::emitOS(SCBE_CPU& pp)
{
    auto& concat = pp.concat;

    if (g_CommandLine.target.os == SwagTargetOs::Windows)
    {
        // :ChkStk Stack probing
        // See SWAG_LIMIT_PAGE_STACK
        if (g_CommandLine.target.arch == SwagTargetArch::X86_64)
        {
            const auto cpuFct = pp.addFunction(R"(__chkstk)", CallConv::get(CallConvKind::X86_64), nullptr);
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
            cpuFct->endAddress = concat.totalCount();
        }
        else
        {
            SWAG_ASSERT(false);
        }

        // int _DllMainCRTStartup(void*, int, void*)
        pp.getOrAddSymbol("_DllMainCRTStartup", CPUSymbolKind::Function, concat.totalCount() - pp.textSectionOffset);
        pp.emitLoad(CPUReg::RAX, 1, OpBits::B64);
        pp.emitRet();
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void SCBE::emitMain(SCBE_CPU& pp)
{
    const auto& buildParameters = pp.buildParams;
    const auto  module          = pp.module;

    const char*     entryPoint = nullptr;
    const CallConv* callConv   = nullptr;
    switch (g_CommandLine.target.os)
    {
        case SwagTargetOs::Windows:
            if (buildParameters.buildCfg->backendSubKind == BuildCfgBackendSubKind::Console)
                entryPoint = "mainCRTStartup";
            else
                entryPoint = "WinMainCRTStartup";
            callConv = CallConv::get(CallConvKind::X86_64);
            break;
        default:
            SWAG_ASSERT(false);
            return;
    }

    pp.cpuFct = pp.addFunction(entryPoint, callConv, nullptr);
    emitEnter(pp,0);

    // Set default system allocator function
    SWAG_ASSERT(g_SystemAllocatorTable);
    const auto bcAlloc = static_cast<ByteCode*>(ByteCode::undoByteCodeLambda(static_cast<void**>(g_SystemAllocatorTable)[0]));
    SWAG_ASSERT(bcAlloc);
    pp.emitSymbolRelocationAddress(CPUReg::RAX, pp.symDefaultAllocTable, 0);
    pp.emitLoadAddress(CPUReg::RCX, CPUReg::RIP, 0);
    pp.emitSymbolRelocationRef(bcAlloc->getCallName());
    pp.emitStore(CPUReg::RAX, 0, CPUReg::RCX, OpBits::B64);

    // mainContext.allocator.itable = &defaultAllocTable;
    pp.emitSymbolRelocationAddress(CPUReg::RCX, pp.symMC_mainContext_allocator_itable, 0);
    pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, OpBits::B64);

    // main context flags
    pp.emitSymbolRelocationAddress(CPUReg::RCX, pp.symMC_mainContext_flags, 0);
    const uint64_t contextFlags = getDefaultContextFlags(module);
    pp.emitStore(CPUReg::RCX, 0, contextFlags, OpBits::B64);

    //__process_infos.contextTlsId = swag_runtime_tlsAlloc();
    pp.emitSymbolRelocationAddress(CPUReg::RDI, pp.symPI_contextTlsId, 0);
    emitInternalCallRAParams(pp, g_LangSpec->name_priv_tlsAlloc, {}, CPUReg::RDI, 0);

    //__process_infos.modules
    pp.emitSymbolRelocationAddress(CPUReg::RCX, pp.symPI_modulesAddr, 0);
    pp.emitSymbolRelocationAddress(CPUReg::RAX, pp.symCSIndex, module->modulesSliceOffset);
    pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, OpBits::B64);
    pp.emitSymbolRelocationAddress(CPUReg::RAX, pp.symPI_modulesCount, 0);
    pp.emitStore(CPUReg::RAX, 0, module->moduleDependencies.count + 1, OpBits::B64);

    //__process_infos.args
    pp.emitClear(CPUReg::RCX, OpBits::B64);
    pp.emitSymbolRelocationAddress(CPUReg::RAX, pp.symPI_argsAddr, 0);
    pp.emitStore(CPUReg::RAX, 0, CPUReg::RCX, OpBits::B64);
    pp.emitSymbolRelocationAddress(CPUReg::RAX, pp.symPI_argsCount, 0);
    pp.emitStore(CPUReg::RAX, 0, CPUReg::RCX, OpBits::B64);

    // Set main context
    pp.emitSymbolRelocationAddress(CPUReg::RAX, pp.symMC_mainContext, 0);
    pp.emitSymbolRelocationAddress(CPUReg::RCX, pp.symPI_defaultContext, 0);
    pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, OpBits::B64);

    // Set current backend as SCBE
    pp.emitSymbolRelocationAddress(CPUReg::RCX, pp.symPI_backendKind, 0);
    pp.emitStore(CPUReg::RCX, 0, static_cast<uint32_t>(SwagBackendGenType::SCBE), OpBits::B32);

    // Set default context in TLS
    pp.pushParams.clear();
    pp.pushParams.push_back({.type = CPUPushParamType::SymbolRelocationValue, .value = pp.symPI_contextTlsId});
    pp.pushParams.push_back({.type = CPUPushParamType::SymbolRelocationValue, .value = pp.symPI_defaultContext});
    emitInternalCallCPUParams(pp, g_LangSpec->name_priv_tlsSetValue, pp.pushParams);

    // Setup runtime
    const auto rtFlags = getRuntimeFlags();
    pp.pushParams.clear();
    pp.pushParams.push_back({.type = CPUPushParamType::Constant, .value = rtFlags, .typeInfo = g_TypeMgr->typeInfoU64});
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
            pp.pushParams.push_back({.type = CPUPushParamType::GlobalString, .value = reinterpret_cast<uint64_t>(nameLib.cstr())});
            pp.pushParams.push_back({.type = CPUPushParamType::Constant, .value = nameLib.length()});
            emitInternalCallCPUParams(pp, g_LangSpec->name_priv_loaddll, pp.pushParams);
        }
    }

    pp.pushParams.clear();
    pp.pushParams.push_back({.type = CPUPushParamType::SymbolRelocationAddress, .value = pp.symPI_processInfos});

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

            pp.emitCallLocal(bc->getCallName());
        }
    }

    // Call to main
    if (module->byteCodeMainFunc)
    {
        pp.emitCallLocal(module->byteCodeMainFunc->getCallName());
    }

    // Call to global drop of this module
    const auto thisDrop = module->getGlobalPrivateFct(g_LangSpec->name_globalDrop);
    pp.emitCallLocal(thisDrop);

    // Call to global drop of all dependencies
    for (uint32_t i = moduleDependencies.size() - 1; i != UINT32_MAX; i--)
    {
        const auto dep = moduleDependencies[i];
        if (!dep->module->isSwag)
            continue;
        auto nameFct = dep->module->getGlobalPrivateFct(g_LangSpec->name_globalDrop);
        pp.emitCallLocal(nameFct);
    }

    pp.emitCallLocal(g_LangSpec->name_priv_closeRuntime);

    pp.emitClear(CPUReg::RAX, OpBits::B64);
    
    emitLeave(pp);
    pp.endFunction();
}

void SCBE::emitGetTypeTable(SCBE_CPU& pp)
{
    const auto& buildParameters = pp.buildParams;
    const auto  module          = pp.module;
    if (buildParameters.buildCfg->backendKind != BuildCfgBackendKind::Library)
        return;

    const auto& cc       = g_TypeMgr->typeInfoModuleCall->getCallConv();
    const auto  thisInit = module->getGlobalPrivateFct(g_LangSpec->name_getTypeTable);
    pp.cpuFct            = pp.addFunction(thisInit, &cc, nullptr);
    emitEnter(pp, 0);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
        pp.directives += form("/EXPORT:%s ", thisInit.cstr());

    pp.emitSymbolRelocationAddress(cc.returnByRegisterInteger, pp.symCSIndex, module->typesSliceOffset);
    
    emitLeave(pp);
    pp.endFunction();
}

void SCBE::emitGlobalPreMain(SCBE_CPU& pp)
{
    const auto& buildParameters = pp.buildParams;
    const auto  module          = pp.module;

    const auto& cc       = g_TypeMgr->typeInfoModuleCall->getCallConv();
    const auto  thisInit = module->getGlobalPrivateFct(g_LangSpec->name_globalPreMain);
    pp.cpuFct            = pp.addFunction(thisInit, &cc, nullptr);

    pp.cpuFct->unwindRegs.push_back(CPUReg::RDI);
    emitEnter(pp, 0);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
        pp.directives += form("/EXPORT:%s ", thisInit.cstr());

    // Store first parameter on stack (process infos ptr)
    SWAG_ASSERT(cc.paramByRegisterCount >= 1);
    pp.emitLoadAddress(CPUReg::RDI, CPUReg::RSP, 0);
    pp.emitStore(CPUReg::RDI, 0, cc.paramByRegisterInteger[0], OpBits::B64);

    // Copy process infos passed as a parameter to the process info struct of this module
    pp.pushParams.clear();
    pp.pushParams.push_back({.type = CPUPushParamType::SymbolRelocationAddress, .value = pp.symPI_processInfos});
    pp.pushParams.push_back({.type = CPUPushParamType::SwagRegister, .baseReg = CPUReg::RDI, .value = REG_OFFSET(0)});
    pp.pushParams.push_back({.type = CPUPushParamType::Constant, .value = sizeof(SwagProcessInfos)});
    emitInternalCallCPUParams(pp, g_LangSpec->name_memcpy, pp.pushParams);

    // Call to #premain functions
    for (const auto bc : module->byteCodePreMainFunc)
    {
        const auto node = bc->node;
        if (node && node->hasAttribute(ATTRIBUTE_COMPILER))
            continue;
        pp.emitCallLocal(bc->getCallName());
    }

    emitLeave(pp);
    pp.endFunction();
}

void SCBE::emitGlobalInit(SCBE_CPU& pp)
{
    const auto& buildParameters = pp.buildParams;
    const auto  module          = pp.module;

    const auto& cc       = g_TypeMgr->typeInfoModuleCall->getCallConv();
    const auto  thisInit = module->getGlobalPrivateFct(g_LangSpec->name_globalInit);
    pp.cpuFct            = pp.addFunction(thisInit, &cc, nullptr);

    pp.cpuFct->unwindRegs.push_back(CPUReg::RDI);
    emitEnter(pp, 0);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
        pp.directives += form("/EXPORT:%s ", thisInit.cstr());

    // Store first parameter on stack (process infos ptr)
    SWAG_ASSERT(cc.paramByRegisterCount >= 1);
    pp.emitLoadAddress(CPUReg::RDI, CPUReg::RSP, 0);
    pp.emitStore(CPUReg::RDI, 0, cc.paramByRegisterInteger[0], OpBits::B64);

    // Copy process infos passed as a parameter to the process info struct of this module
    pp.pushParams.clear();
    pp.pushParams.push_back({.type = CPUPushParamType::SymbolRelocationAddress, .value = pp.symPI_processInfos});
    pp.pushParams.push_back({.type = CPUPushParamType::SwagRegister, .baseReg = CPUReg::RDI, .value = REG_OFFSET(0)});
    pp.pushParams.push_back({.type = CPUPushParamType::Constant, .value = sizeof(SwagProcessInfos)});
    emitInternalCallCPUParams(pp, g_LangSpec->name_memcpy, pp.pushParams);

    // Thread local storage
    pp.emitSymbolRelocationAddress(CPUReg::RDI, pp.symTls_threadLocalId, 0);
    pp.pushParams.clear();
    emitInternalCallCPUParams(pp, g_LangSpec->name_priv_tlsAlloc, pp.pushParams, CPUReg::RDI, 0);

    // Init type table slice for each dependency (by calling ???_getTypeTable)
    pp.emitSymbolRelocationAddress(CPUReg::RCX, pp.symCSIndex, module->modulesSliceOffset + sizeof(SwagModule) + offsetof(SwagModule, types));
    for (const auto& dep : module->moduleDependencies)
    {
        if (!dep->module->isSwag)
        {
            pp.emitOpBinary(CPUReg::RCX, sizeof(SwagModule), CPUOp::ADD, OpBits::B64);
            continue;
        }

        auto callTable = dep->module->getGlobalPrivateFct(g_LangSpec->name_getTypeTable);
        pp.emitCallLocal(callTable);

        // Count types is stored as a uint64_t at the start of the address
        pp.emitLoad(CPUReg::R8, cc.returnByRegisterInteger, 0, OpBits::B64);
        pp.emitStore(CPUReg::RCX, sizeof(uint64_t), CPUReg::R8, OpBits::B64);
        pp.emitOpBinary(cc.returnByRegisterInteger, sizeof(uint64_t), CPUOp::ADD, OpBits::B64);
        pp.emitStore(CPUReg::RCX, 0, cc.returnByRegisterInteger, OpBits::B64);

        pp.emitOpBinary(CPUReg::RCX, sizeof(SwagModule), CPUOp::ADD, OpBits::B64);
    }

    // Call to #init functions
    for (const auto bc : module->byteCodeInitFunc)
    {
        const auto node = bc->node;
        if (node && node->hasAttribute(ATTRIBUTE_COMPILER))
            continue;
        pp.emitCallLocal(bc->getCallName());
    }

    emitLeave(pp);
    pp.endFunction();
}

void SCBE::emitGlobalDrop(SCBE_CPU& pp)
{
    const auto  module          = pp.module;
    const auto& buildParameters = pp.buildParams;

    const auto& cc       = g_TypeMgr->typeInfoModuleCall->getCallConv();
    const auto  thisDrop = module->getGlobalPrivateFct(g_LangSpec->name_globalDrop);
    pp.cpuFct            = pp.addFunction(thisDrop, &cc, nullptr);
    emitEnter(pp, 0);

    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
        pp.directives += form("/EXPORT:%s ", thisDrop.cstr());

    // Call to #drop functions
    for (const auto bc : module->byteCodeDropFunc)
    {
        const auto node = bc->node;
        if (node && node->hasAttribute(ATTRIBUTE_COMPILER))
            continue;
        pp.emitCallLocal(bc->getCallName());
    }

    // __dropGlobalVariables
    emitInternalCallCPUParams(pp, g_LangSpec->name_priv_dropGlobalVariables, pp.pushParams);

    emitLeave(pp);
    pp.endFunction();
}
