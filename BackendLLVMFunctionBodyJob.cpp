#include "pch.h"
#include "BackendLLVMFunctionBodyJob.h"
#include "BackendLLVM.h"
#include "ByteCode.h"
#include "Ast.h"
#include "Module.h"

thread_local Pool<BackendLLVMFunctionBodyJob> g_Pool_backendLLVMFunctionBodyJob;

JobResult BackendLLVMFunctionBodyJob::execute()
{
    BackendLLVM* bachendLLVM = (BackendLLVM*) backend;

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
        bachendLLVM->emitFunctionBody(buildParameters, module, one);

        // Emit public function wrapper, from real C prototype to swag registers
        if (node && node->attributeFlags & ATTRIBUTE_PUBLIC)
            bachendLLVM->emitFuncWrapperPublic(buildParameters, module, typeFunc, node, one);
    }

    return JobResult::ReleaseJob;
}
