#include "pch.h"
#include "BackendX64.h"
#include "BackendX64FunctionBodyJob.h"
#include "Module.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "TypeManager.h"
#include "Ast.h"

BackendFunctionBodyJob* BackendX64::newFunctionJob()
{
    return g_Pool_backendX64FunctionBodyJob.alloc();
}

bool BackendX64::emitFuncWrapperPublic(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* bc)
{
    return true;
}

bool BackendX64::emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc)
{
    if (bc->node && (bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC))
    {
        if (buildParameters.compileType != BackendCompileType::Test)
            return true;
    }

    return true;
}
