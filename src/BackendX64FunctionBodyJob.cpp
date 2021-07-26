#include "pch.h"
#include "BackendX64FunctionBodyJob.h"
#include "BackendX64.h"
#include "Workspace.h"
#include "ByteCode.h"
#include "Ast.h"
#include "ThreadManager.h"
#include "Module.h"
#include "Timer.h"

JobResult BackendX64FunctionBodyJob::execute()
{
    Timer timer0{&g_Stats.prepOutputTimeJob};
    Timer timer1{&g_Stats.prepOutputTimeJob_GenFunc};
    timer0.start();
    timer1.start();

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
        if (node && node->attributeFlags & (ATTRIBUTE_PUBLIC | ATTRIBUTE_CALLBACK))
        {
            if (!bachendX64->emitFuncWrapperPublic(buildParameters, module, typeFunc, node, one))
                return JobResult::ReleaseJob;
        }
    }

    timer1.stop();
    timer0.stop();
    return JobResult::ReleaseJob;
}
