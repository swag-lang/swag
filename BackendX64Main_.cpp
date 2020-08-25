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
    concat.addString3("\x48\x8d\x0d"); // lea rcx, qword ptr ????????[rip]
    BackendX64Inst::emit_Symbol_Relocation(pp, pp.symPI_args_addr);
    emitCall(pp, "swag_runtime_convertArgcArgv");

    //static swag_allocator_t defaultAllocTable = &swag_SystemAllocator_alloc_6E46EF68;
    SWAG_ASSERT(g_defaultContext.allocator.itable);
    auto bcAlloc = (ByteCode*) undoByteCodeLambda(((void**) g_defaultContext.allocator.itable)[0]);
    SWAG_ASSERT(bcAlloc);
    BackendX64Inst::emit_SymbolAddr_In_RAX(pp, pp.symDefaultAllocTable);
    concat.addString3("\x48\x8d\x0d"); // lea rcx, qword ptr ????????[rip]
    emitSymbolRelocation(pp, bcAlloc->callName());
    concat.addString3("\x48\x89\x08"); // mov [rax], rcx

    //mainContext.allocator.itable = &defaultAllocTable;
    concat.addString3("\x48\x8d\x0d"); // lea rcx, qword ptr ????????[rip]
    BackendX64Inst::emit_Symbol_Relocation(pp, pp.symMC_mainContext_allocator_itable);
    concat.addString3("\x48\x89\x01"); // mov [rcx], rax

    //__process_infos.contextTlsId = swag_runtime_tlsAlloc();
    emitCall(pp, "swag_runtime_tlsAlloc");
    concat.addString3("\x48\x8d\x0d"); // lea rcx, qword ptr ????????[rip]
    BackendX64Inst::emit_Symbol_Relocation(pp, pp.symPI_contextTlsId);
    concat.addString3("\x48\x89\x01"); // mov [rcx], rax

    //__process_infos.defaultContext = &mainContext;
    BackendX64Inst::emit_SymbolAddr_In_RAX(pp, pp.symMC_mainContext);
    concat.addString3("\x48\x8d\x0d"); // lea rcx, qword ptr ????????[rip]
    BackendX64Inst::emit_Symbol_Relocation(pp, pp.symPI_defaultContext);
    concat.addString3("\x48\x89\x01"); // mov [rcx], rax

    //swag_runtime_tlsSetValue(__process_infos.contextTlsId, __process_infos.defaultContext);
    concat.addString3("\x48\x8b\x0d"); // mov rcx, qword ptr ????????[rip]
    BackendX64Inst::emit_Symbol_Relocation(pp, pp.symPI_contextTlsId);
    concat.addString3("\x48\x8b\x15"); // mov rdx, qword ptr ????????[rip]
    BackendX64Inst::emit_Symbol_Relocation(pp, pp.symPI_defaultContext);

    emitCall(pp, "swag_runtime_tlsSetValue");

    // Call to global init of this module
    concat.addString3("\x48\x8d\x0d"); // mov rcx, qword ptr ????????[rip]
    BackendX64Inst::emit_Symbol_Relocation(pp, pp.symPI_processInfos);
    auto thisInit = format("%s_globalInit", module->nameDown.c_str());
    emitCall(pp, thisInit);

    // Call to global init of all dependencies
    for (const auto& dep : module->moduleDependencies)
    {
        auto nameDown = dep->name;
        nameDown.replaceAll('.', '_');
        emitGlobalString(pp, precompileIndex, nameDown, RCX);
        emitCall(pp, "swag_runtime_loadDynamicLibrary");

        if (dep->generated)
        {
            concat.addString3("\x48\x8d\x0d"); // mov rcx, qword ptr ????????[rip]
            BackendX64Inst::emit_Symbol_Relocation(pp, pp.symPI_processInfos);
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
    concat.addU8(0xC3); // ret
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
    concat.addString3("\x48\x89\xca"); // mov rdx, rcx
    concat.addString3("\x48\x8d\x0d"); // mov rcx, qword ptr ????????[rip]
    BackendX64Inst::emit_Symbol_Relocation(pp, pp.symPI_processInfos);
    BackendX64Inst::emit_Load64_Immediate(pp, sizeof(swag_process_infos_t), R8);
    emitCall(pp, "memcpy");

    // Call to #init functions
    for (auto bc : module->byteCodeInitFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        emitCall(pp, bc->callName());
    }

    BackendX64Inst::emit_Add_Cst32_To_RSP(pp, 40);
    concat.addU8(0xC3); // ret

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
    concat.addU8(0xC3); // ret

    return true;
}