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

    getOrAddSymbol(pp, "main", CoffSymbolKind::Function, concat.totalCount - pp.textSectionOffset);

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

    BackendX64Inst::emit(concat, BackendX64Inst::Xor, BackendX64Inst::EAX_EAX);
    BackendX64Inst::emit(concat, BackendX64Inst::Ret);
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