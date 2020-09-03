#include "pch.h"
#include "BackendX64.h"
#include "Module.h"
#include "AstNode.h"
#include "ByteCode.h"
#include "Context.h"
#include "BackendX64Inst.h"

bool BackendX64::emitMain(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    getOrAddSymbol(pp, "main", CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset);
    BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, 40);

    //swag_runtime_convertArgcArgv(&__process_infos.arguments, argc, (void**)argv);
    // Must be done first ! We need to have rcx (argc) and rdx (argv) valid
    concat.addString3("\x49\x89\xd0"); // mov r8, rdx -- argv
    concat.addString3("\x48\x89\xca"); // mov rdx, rcx -- argc
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symPI_args_addr, 0);
    emitCall(pp, "swag_runtime_convertArgcArgv");

    //static swag_allocator_t defaultAllocTable = &swag_SystemAllocator_alloc_6E46EF68;
    SWAG_ASSERT(g_defaultContext.allocator.itable);
    auto bcAlloc = (ByteCode*) undoByteCodeLambda(((void**) g_defaultContext.allocator.itable)[0]);
    SWAG_ASSERT(bcAlloc);
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RAX, pp.symDefaultAllocTable, 0);
    concat.addString3("\x48\x8d\x0d"); // lea rcx, qword ptr ????????[rip]
    emitSymbolRelocation(pp, bcAlloc->callName());
    concat.addString3("\x48\x89\x08"); // mov [rax], rcx

    //mainContext.allocator.itable = &defaultAllocTable;
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symMC_mainContext_allocator_itable, 0);
    concat.addString3("\x48\x89\x01"); // mov [rcx], rax

    // main context flags
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symMC_mainContext_flags, 0);
    uint64_t contextFlags = getDefaultContextFlags(module);
    BackendX64Inst::emit_Store64_Immediate(pp, 0, contextFlags, RCX);

    //__process_infos.contextTlsId = swag_runtime_tlsAlloc();
    emitCall(pp, "swag_runtime_tlsAlloc");
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symPI_contextTlsId, 0);
    concat.addString3("\x48\x89\x01"); // mov [rcx], rax

    //__process_infos.defaultContext = &mainContext;
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RAX, pp.symMC_mainContext, 0);
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symPI_defaultContext, 0);
    concat.addString3("\x48\x89\x01"); // mov [rcx], rax

    //swag_runtime_tlsSetValue(__process_infos.contextTlsId, __process_infos.defaultContext);
    BackendX64Inst::emit_Symbol_RelocationValue(pp, RCX, pp.symPI_contextTlsId, 0);
    BackendX64Inst::emit_Symbol_RelocationValue(pp, RDX, pp.symPI_defaultContext, 0);
    emitCall(pp, "swag_runtime_tlsSetValue");

    // Call to global init of this module
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symPI_processInfos, 0);
    auto thisInit = format("%s_globalInit", module->nameDown.c_str());
    emitCall(pp, thisInit);

    // Call to global init of all dependencies
    for (const auto& dep : module->moduleDependencies)
    {
        auto nameDown = dep->name;
        nameDown.replaceAll('.', '_');
        auto nameLib = nameDown + OS::getDllFileExtension();
        emitGlobalString(pp, precompileIndex, nameLib, RCX);
        emitCall(pp, "swag_runtime_loadDynamicLibrary");

        if (dep->generated)
        {
            BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symPI_processInfos, 0);
            auto initFunc = format("%s_globalInit", nameDown.c_str());
            emitCall(pp, initFunc);
        }
    }

    // Call to test functions
    if (buildParameters.compileType == BackendCompileType::Test && !module->byteCodeTestFunc.empty())
    {
        for (auto bc : module->byteCodeTestFunc)
        {
            auto node = bc->node;
            if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;
            emitCall(pp, bc->callName());
        }
    }

    // Call to main
    if (module->byteCodeMainFunc)
    {
        emitCall(pp, module->byteCodeMainFunc->callName());
    }

    // Call to global drop of this module
    auto thisDrop = format("%s_globalDrop", module->nameDown.c_str());
    emitCall(pp, thisDrop);

    // Call to global drop of all dependencies
    for (const auto& dep : module->moduleDependencies)
    {
        if (!dep->generated)
            continue;
        auto nameDown = dep->name;
        nameDown.replaceAll('.', '_');
        auto funcDrop = format("%s_globalDrop", nameDown.c_str());
        emitCall(pp, funcDrop);
    }

    BackendX64Inst::emit_Clear64(pp, RAX);
    BackendX64Inst::emit_Add_Cst32_To_RSP(pp, 40);
    BackendX64Inst::emit_Ret(pp);
    return true;
}

bool BackendX64::emitGlobalInit(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    auto thisInit = format("%s_globalInit", module->nameDown.c_str());
    getOrAddSymbol(pp, thisInit, CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset);
    pp.directives += format("/EXPORT:%s ", thisInit.c_str());

    BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, 40);

    // __process_infos = *processInfos;
    BackendX64Inst::emit_Copy64(pp, RCX, RDX);
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symPI_processInfos, 0);
    BackendX64Inst::emit_Load64_Immediate(pp, sizeof(SwagProcessInfos), R8);
    emitCall(pp, "memcpy");

    // Inform runtime about my processInfos
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symPI_processInfos, 0);
    emitCall(pp, "swag_runtime_setProcessInfos");

    // Call to #init functions
    for (auto bc : module->byteCodeInitFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        emitCall(pp, bc->callName());
    }

    BackendX64Inst::emit_Add_Cst32_To_RSP(pp, 40);
    BackendX64Inst::emit_Ret(pp);
    return true;
}

bool BackendX64::emitGlobalDrop(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    auto thisDrop = format("%s_globalDrop", module->nameDown.c_str());
    getOrAddSymbol(pp, thisDrop, CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset);
    pp.directives += format("/EXPORT:%s ", thisDrop.c_str());

    BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, 40);

    // Call to #drop functions
    for (auto bc : module->byteCodeDropFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        emitCall(pp, bc->callName());
    }

    BackendX64Inst::emit_Add_Cst32_To_RSP(pp, 40);
    BackendX64Inst::emit_Ret(pp);
    return true;
}