#include "pch.h"
#include "BackendX64.h"
#include "Module.h"
#include "AstNode.h"
#include "ByteCode.h"
#include "Context.h"

bool BackendX64::emitMain(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    pp.allSymbols.push_back({"main", CoffSymbolKind::Function, concat.totalCount - pp.textSectionOffset});

    concat.addU16(0xC033); // XOR EAX, EAX

    concat.addU8(0xC3); // RET

    // This is it. The last function in the .text segment. Compute its size
    applyPatch(pp, PatchType::TextSectionSize, concat.totalCount - pp.textSectionOffset);
    return true;
}

bool BackendX64::emitGlobalInit(const BuildParameters& buildParameters)
{
    return true;
}

bool BackendX64::emitGlobalDrop(const BuildParameters& buildParameters)
{
    return true;
}