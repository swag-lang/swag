#include "pch.h"
#include "BackendX64.h"
#include "Module.h"
#include "AstNode.h"
#include "ByteCode.h"
#include "Context.h"
#include "BackendX64Inst.h"
#include "Workspace.h"

bool BackendX64::emitOS(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    if (g_CommandLine.os == BackendOs::Windows)
    {
        // int _DllMainCRTStartup(void*, int, void*)
        getOrAddSymbol(pp, "_DllMainCRTStartup", CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset);
        BackendX64Inst::emit_Load64_Immediate(pp, 1, RAX);
        BackendX64Inst::emit_Ret(pp);
        return true;
    }
    else
    {
        module->error(format("llvm backend unsupported os '%s'", g_Workspace.GetOsName().c_str()));
        return false;
    }
}

bool BackendX64::emitMain(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    SWAG_CHECK(emitOS(buildParameters));

    const char* entryPoint = nullptr;
    switch (g_CommandLine.os)
    {
    case BackendOs::Windows:
        entryPoint = "mainCRTStartup";
        break;
    default:
        module->error(format("x64 backend unsupported os '%s'", g_Workspace.GetOsName().c_str()));
        return false;
    }

    getOrAddSymbol(pp, entryPoint, CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset);
    BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, 40);

    //static swag_allocator_t defaultAllocTable = &swag_SystemAllocator_alloc_6E46EF68;
    SWAG_ASSERT(g_defaultContext.allocator.itable);
    auto bcAlloc = (ByteCode*) undoByteCodeLambda(((void**) g_defaultContext.allocator.itable)[0]);
    SWAG_ASSERT(bcAlloc);
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RAX, pp.symDefaultAllocTable, 0);
    concat.addString3("\x48\x8d\x0d"); // lea rcx, qword ptr ????????[rip]
    emitSymbolRelocation(pp, bcAlloc->callName());
    BackendX64Inst::emit_Store64_Indirect(pp, 0, RCX, RAX);

    //mainContext.allocator.itable = &defaultAllocTable;
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symMC_mainContext_allocator_itable, 0);
    BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RCX);

    // main context flags
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symMC_mainContext_flags, 0);
    uint64_t contextFlags = getDefaultContextFlags(module);
    BackendX64Inst::emit_Store64_Immediate(pp, 0, contextFlags, RCX);

    //__process_infos.contextTlsId = swag_runtime_tlsAlloc();
    emitCall(pp, "swag_runtime_tlsAlloc");
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symPI_contextTlsId, 0);
    BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RCX);

    //__process_infos.defaultContext = &mainContext;
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RAX, pp.symMC_mainContext, 0);
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symPI_defaultContext, 0);
    BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RCX);

    //swag_runtime_tlsSetValue(__process_infos.contextTlsId, __process_infos.defaultContext);
    BackendX64Inst::emit_Symbol_RelocationValue(pp, RCX, pp.symPI_contextTlsId, 0);
    BackendX64Inst::emit_Symbol_RelocationValue(pp, RDX, pp.symPI_defaultContext, 0);
    emitCall(pp, "swag_runtime_tlsSetValue");

    // Load all dependencies
    for (const auto& dep : module->moduleDependencies)
    {
        auto nameDown = dep->name;
        Ast::normalizeIdentifierName(nameDown);
        auto nameLib = nameDown + OS::getDllFileExtension();
        emitGlobalString(pp, precompileIndex, nameLib, RAX);
        BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RSP);
        BackendX64Inst::emit_Load64_Immediate(pp, nameLib.length(), RAX);
        BackendX64Inst::emit_Store64_Indirect(pp, 8, RAX, RSP);
        emitCall(pp, "__swag_runtime_loaddll");
    }

    // Call to global init of all dependencies
    for (const auto& dep : module->moduleDependencies)
    {
        SWAG_ASSERT(dep->module);
        if (!dep->generated || !dep->module->mustOutputSomething())
            continue;
        auto nameDown = dep->name;
        Ast::normalizeIdentifierName(nameDown);
        BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symPI_processInfos, 0);
        auto initFunc = format("%s_globalInit", nameDown.c_str());
        emitCall(pp, initFunc);
    }

    // Call to global init of this module
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symPI_processInfos, 0);
    auto thisInit = format("%s_globalInit", module->nameDown.c_str());
    emitCall(pp, thisInit);

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
        if (!dep->generated || !dep->module->mustOutputSomething())
            continue;
        auto nameDown = dep->name;
        Ast::normalizeIdentifierName(nameDown);
        auto funcDrop = format("%s_globalDrop", nameDown.c_str());
        emitCall(pp, funcDrop);
    }

    BackendX64Inst::emit_Clear64(pp, RAX);
    BackendX64Inst::emit_Add_Cst32_To_RSP(pp, 40);
    emitCall(pp, "__swag_runtime_exit");
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

    // Reloc functions
    for (auto& k : module->constantSegment.initFuncPtr)
    {
        auto relocType = k.second.second;
        if (relocType != DataSegment::RelocType::Foreign)
            continue;

        // Load address of the foreign function in RAX
        CoffRelocation reloc;
        BackendX64Inst::emit_Load64_Immediate(pp, 0, RAX, true);
        reloc.virtualAddress = concat.totalCount() - sizeof(uint64_t) - pp.textSectionOffset;
        auto callSym         = getOrAddSymbol(pp, k.second.first, CoffSymbolKind::Extern);
        reloc.symbolIndex    = callSym->index;
        reloc.type           = IMAGE_REL_AMD64_ADDR64;
        pp.relocTableTextSection.table.push_back(reloc);

        // Add the foreign marker
        BackendX64Inst::emit_Load64_Immediate(pp, SWAG_LAMBDA_FOREIGN_MARKER, RCX);
        BackendX64Inst::emit_Op64(pp, RCX, RAX, X64Op::OR);

        // Then store the result in the constant buffer
        BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symCSIndex, 0);
        BackendX64Inst::emit_Store64_Indirect(pp, k.first, RAX, RCX);
    }

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