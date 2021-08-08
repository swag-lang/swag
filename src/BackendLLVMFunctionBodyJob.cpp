#include "pch.h"
#include "BackendLLVMFunctionBodyJob.h"
#include "BackendLLVM.h"
#include "ByteCode.h"
#include "Ast.h"
#include "Module.h"
#include "Timer.h"

JobResult BackendLLVMFunctionBodyJob::execute()
{
    Timer timer0{&g_Stats.prepOutputTimeJob};
    Timer timer1{&g_Stats.prepOutputTimeJob_GenFunc};

    BackendLLVM* bachendLLVM = (BackendLLVM*) backend;

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
        if (!bachendLLVM->emitFunctionBody(buildParameters, module, one))
            return JobResult::ReleaseJob;

        // Emit public function wrapper, from real C prototype to swag registers
        if (node && node->attributeFlags & (ATTRIBUTE_PUBLIC | ATTRIBUTE_CALLBACK))
        {
            if (!bachendLLVM->emitFuncWrapperPublic(buildParameters, module, typeFunc, node, one))
                return JobResult::ReleaseJob;
        }
    }

    return JobResult::ReleaseJob;
}
