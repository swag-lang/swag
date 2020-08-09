#include "pch.h"
#include "BackendX64FunctionBodyJob.h"
#include "BackendX64.h"
#include "Workspace.h"
#include "ByteCode.h"
#include "Ast.h"
#include "ThreadManager.h"
#include "Module.h"

thread_local Pool<BackendX64FunctionBodyJob> g_Pool_backendX64FunctionBodyJob;

JobResult BackendX64FunctionBodyJob::execute()
{
    BackendX64* bachendX64 = (BackendX64*) backend;

    for (auto one : byteCodeFunc)
    {
        TypeInfoFuncAttr* typeFunc = one->typeInfoFunc;
        AstFuncDecl*      node     = nullptr;

        if (one->node)
        {
            node     = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);
            typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
        }

        // Emit the internal function
        bachendX64->emitFunctionBody(buildParameters, module, one);

        // Emit public function wrapper, from real C prototype to swag registers
        if (node && node->attributeFlags & ATTRIBUTE_PUBLIC)
            bachendX64->emitFuncWrapperPublic(buildParameters, module, typeFunc, node, one);
    }

    return JobResult::ReleaseJob;
}
