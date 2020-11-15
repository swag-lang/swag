#include "pch.h"
#include "BackendX64FunctionBodyJob.h"
#include "BackendX64.h"
#include "Workspace.h"
#include "ByteCode.h"
#include "Ast.h"
#include "ThreadManager.h"
#include "Module.h"
#include "Profile.h"

thread_local Pool<BackendX64FunctionBodyJob> g_Pool_backendX64FunctionBodyJob;

JobResult BackendX64FunctionBodyJob::execute()
{
    SWAG_PROFILE(PRF_GFCT, format("x64 emit functions %s", module->name.c_str()));

    BackendX64* bachendX64 = (BackendX64*) backend;

    for (auto one : byteCodeFunc)
    {
        TypeInfoFuncAttr* typeFunc = one->typeInfoFunc;
        AstFuncDecl*      node     = nullptr;

        if (one->node)
        {
            if (one->node->attributeFlags & (ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO))
                continue;
            node     = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);
            typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
        }

        // Emit the internal function
        if (!bachendX64->emitFunctionBody(buildParameters, module, one))
            return JobResult::ReleaseJob;

        // Emit public function wrapper, from real C prototype to swag registers
        if (node && node->attributeFlags & ATTRIBUTE_PUBLIC)
        {
            if(!bachendX64->emitFuncWrapperPublic(buildParameters, module, typeFunc, node, one))
                return JobResult::ReleaseJob;
        }
    }

    return JobResult::ReleaseJob;
}
