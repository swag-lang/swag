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

    // Call to global init of this module
    auto thisInit = format("%s_globalInit", module->nameDown.c_str());
    emitCall(pp, thisInit);

    // Call to global init of all dependencies
    for (const auto& dep : module->moduleDependencies)
    {
        auto nameDown = dep.first;
        nameDown.replaceAll('.', '_');
        concat.addString2("\x48\xb9"); // mov rcx, ????????_????????
        emitGlobalString(pp, precompileIndex, nameDown);
        emitCall(pp, "swag_runtime_loadDynamicLibrary");

        if (dep.second.generated)
        {
            //processInfos
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
        if (!dep.second.generated)
            continue;
        auto nameDown = dep.first;
        nameDown.replaceAll('.', '_');
        auto funcDrop = format("%s_globalDrop", nameDown.c_str());
        emitCall(pp, funcDrop);
    }

    BackendX64Inst::emit_Clear_RAX(pp);
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

    // Call to #init functions
    for (auto bc : module->byteCodeInitFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        emitCall(pp, bc->callName());
    }

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

    // Call to #drop functions
    for (auto bc : module->byteCodeDropFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        emitCall(pp, bc->callName());
    }

    concat.addU8(0xC3); // ret

    return true;
}