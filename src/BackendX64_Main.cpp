#include "pch.h"
#include "BackendX64.h"
#include "Module.h"
#include "AstNode.h"
#include "ByteCode.h"
#include "Context.h"
#include "BackendX64Inst.h"
#include "Workspace.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"

bool BackendX64::emitOS(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    concat.align(16);

    if (g_CommandLine->target.os == SwagTargetOs::Windows)
    {
        // int _DllMainCRTStartup(void*, int, void*)
        getOrAddSymbol(pp, "_DllMainCRTStartup", CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset);
        BackendX64Inst::emit_Load64_Immediate(pp, 1, RAX);
        BackendX64Inst::emit_Ret(pp);
        return true;
    }
    else
    {
        module->error(Utf8::format(g_E[Err0056], Backend::GetOsName(g_CommandLine->target)));
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
    switch (g_CommandLine->target.os)
    {
    case SwagTargetOs::Windows:
        entryPoint = "mainCRTStartup";
        break;
    default:
        module->error(Utf8::format(g_E[Err0056], Backend::GetOsName(g_CommandLine->target)));
        return false;
    }

    auto symbolFuncIndex = getOrAddSymbol(pp, entryPoint, CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    auto coffFct         = registerFunction(pp, nullptr, symbolFuncIndex);

    auto beforeProlog = concat.totalCount();
    BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, 40);
    auto                   sizeProlog = concat.totalCount() - beforeProlog;
    VectorNative<uint16_t> unwind;
    computeUnwindStack(40, sizeProlog, unwind);

    // Set default system allocator function
    SWAG_ASSERT(g_DefaultContext.allocator.itable);
    auto bcAlloc = (ByteCode*) ByteCode::undoByteCodeLambda(((void**) g_DefaultContext.allocator.itable)[0]);
    SWAG_ASSERT(bcAlloc);
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RAX, pp.symDefaultAllocTable, 0);
    concat.addString3("\x48\x8d\x0d"); // lea rcx, qword ptr ????????[rip]
    emitSymbolRelocation(pp, bcAlloc->getCallName());
    BackendX64Inst::emit_Store64_Indirect(pp, 0, RCX, RAX);

    //mainContext.allocator.itable = &defaultAllocTable;
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symMC_mainContext_allocator_itable, 0);
    BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RCX);

    // main context flags
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symMC_mainContext_flags, 0);
    uint64_t contextFlags = getDefaultContextFlags(module);
    BackendX64Inst::emit_Store64_Immediate(pp, 0, contextFlags, RCX);

    //__process_infos.contextTlsId = swag_runtime_tlsAlloc();
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symPI_contextTlsId, 0);
    emitCall(pp, g_LangSpec->name__tlsAlloc);

    // Set main context
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RAX, pp.symMC_mainContext, 0);
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symPI_defaultContext, 0);
    BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RCX);

    // Set current backend as X64
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symPI_backendKind, 0);
    BackendX64Inst::emit_Store32_Immediate(pp, 0, (uint32_t) SwagBackendGenType::X64, RCX);

    // Set default context in TLS
    BackendX64Inst::emit_Symbol_RelocationValue(pp, RCX, pp.symPI_contextTlsId, 0);
    BackendX64Inst::emit_Symbol_RelocationValue(pp, RDX, pp.symPI_defaultContext, 0);
    emitCall(pp, g_LangSpec->name__tlsSetValue);

    // Setup runtime
    emitCall(pp, g_LangSpec->name__setupRuntime);

    // Load all dependencies
    VectorNative<ModuleDependency*> moduleDependencies;
    module->sortDependenciesByInitOrder(moduleDependencies);
    for (int i = 0; i < moduleDependencies.size(); i++)
    {
        auto dep      = moduleDependencies[i];
        auto nameDown = dep->name;
        Ast::normalizeIdentifierName(nameDown);
        auto nameLib = nameDown;
        nameLib += Backend::getOutputFileExtension(g_CommandLine->target, BuildCfgBackendKind::DynamicLib);
        emitGlobalString(pp, precompileIndex, nameLib, RCX);
        BackendX64Inst::emit_Load64_Immediate(pp, nameLib.length(), RDX);
        emitCall(pp, g_LangSpec->name__loaddll);
    }

    // Call to global init of all dependencies
    for (int i = 0; i < moduleDependencies.size(); i++)
    {
        auto dep = moduleDependencies[i];
        SWAG_ASSERT(dep->module);
        if (!dep->module->isSwag)
            continue;
        auto nameDown = dep->name;
        Ast::normalizeIdentifierName(nameDown);
        BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symPI_processInfos, 0);
        auto initFunc = nameDown;
        initFunc += "_globalInit";
        emitCall(pp, initFunc);
    }

    // Call to global init of this module
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symPI_processInfos, 0);
    auto thisInit = Utf8::format("%s_globalInit", module->nameNormalized.c_str());
    emitCall(pp, thisInit);

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
    auto thisDrop = Utf8::format("%s_globalDrop", module->nameNormalized.c_str());
    emitCall(pp, thisDrop);

    // Call to global drop of all dependencies
    for (int i = (int) moduleDependencies.size() - 1; i >= 0; i--)
    {
        auto dep = moduleDependencies[i];
        if (!dep->module->isSwag)
            continue;
        auto nameDown = dep->name;
        Ast::normalizeIdentifierName(nameDown);
        auto funcDrop = nameDown;
        funcDrop += "_globalDrop";
        emitCall(pp, funcDrop);
    }

    emitCall(pp, g_LangSpec->name__exit);

    BackendX64Inst::emit_Clear64(pp, RAX);
    BackendX64Inst::emit_Add_Cst32_To_RSP(pp, 40);
    BackendX64Inst::emit_Ret(pp);

    uint32_t endAddress = concat.totalCount();
    registerFunction(coffFct, startAddress, endAddress, sizeProlog, unwind);
    return true;
}

void BackendX64::emitPatchForeignPointers(const BuildParameters& buildParameters, DataSegment* segment, uint32_t segIndex)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    for (auto& k : segment->initFuncPtr)
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

        // Then store the result in the segment
        BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, segIndex, 0);
        BackendX64Inst::emit_Store64_Indirect(pp, k.first, RAX, RCX);
    }
}

bool BackendX64::emitGlobalInit(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    concat.align(16);
    auto startAddress = concat.totalCount();

    auto thisInit        = Utf8::format("%s_globalInit", module->nameNormalized.c_str());
    auto symbolFuncIndex = getOrAddSymbol(pp, thisInit, CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    auto coffFct         = registerFunction(pp, nullptr, symbolFuncIndex);

    pp.directives += Utf8::format("/EXPORT:%s ", thisInit.c_str());

    auto beforeProlog = concat.totalCount();
    BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, 40);
    auto                   sizeProlog = concat.totalCount() - beforeProlog;
    VectorNative<uint16_t> unwind;
    computeUnwindStack(40, sizeProlog, unwind);

    // Copy process infos
    BackendX64Inst::emit_Copy64(pp, RCX, RDX);
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symPI_processInfos, 0);
    BackendX64Inst::emit_Load64_Immediate(pp, sizeof(SwagProcessInfos), R8);
    emitCall(pp, g_LangSpec->name_memcpy);

    // Thread local storage
    BackendX64Inst::emit_Symbol_RelocationAddr(pp, RCX, pp.symTls_threadLocalId, 0);
    emitCall(pp, g_LangSpec->name__tlsAlloc);

    // Reloc functions
    emitPatchForeignPointers(buildParameters, &module->constantSegment, pp.symCSIndex);
    emitPatchForeignPointers(buildParameters, &module->tlsSegment, pp.symTLSIndex);

    // Call to #init functions
    for (auto bc : module->byteCodeInitFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        emitCall(pp, bc->getCallName());
    }

    BackendX64Inst::emit_Add_Cst32_To_RSP(pp, 40);
    BackendX64Inst::emit_Ret(pp);

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

    auto thisDrop        = Utf8::format("%s_globalDrop", module->nameNormalized.c_str());
    auto symbolFuncIndex = getOrAddSymbol(pp, thisDrop, CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    auto coffFct         = registerFunction(pp, nullptr, symbolFuncIndex);

    pp.directives += Utf8::format("/EXPORT:%s ", thisDrop.c_str());

    auto beforeProlog = concat.totalCount();
    BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, 40);
    auto                   sizeProlog = concat.totalCount() - beforeProlog;
    VectorNative<uint16_t> unwind;
    computeUnwindStack(40, sizeProlog, unwind);

    // Call to #drop functions
    for (auto bc : module->byteCodeDropFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        emitCall(pp, bc->getCallName());
    }

    BackendX64Inst::emit_Add_Cst32_To_RSP(pp, 40);
    BackendX64Inst::emit_Ret(pp);

    uint32_t endAddress = concat.totalCount();
    registerFunction(coffFct, startAddress, endAddress, sizeProlog, unwind);
    return true;
}