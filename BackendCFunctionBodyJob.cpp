#include "pch.h"
#include "BackendCFunctionBodyJob.h"
#include "BackendC.h"
#include "Workspace.h"
#include "ByteCode.h"
#include "Ast.h"

thread_local Pool<BackendCFunctionBodyJob> g_Pool_backendCFunctionBodyJob;
thread_local Concat                        g_Concat;

JobResult BackendCFunctionBodyJob::execute()
{
    g_Concat.clear();

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
        backend->emitFunctionBody(g_Concat, module, one);

        // Emit public function wrapper, from real C prototype to swag registers
        if (node && node->attributeFlags & ATTRIBUTE_PUBLIC)
            backend->emitFuncWrapperPublic(g_Concat, module, typeFunc, node, one);
    }

    auto        firstBucket = g_Concat.firstBucket;
    SaveRequest req;

    // Must save one by one
    static mutex m;
    unique_lock  lk(m);
    while (firstBucket)
    {
        req.buffer     = (char*) firstBucket->datas;
        req.bufferSize = firstBucket->count;
        backend->bufferCFiles[precompileIndex].save(&req);
        firstBucket = firstBucket->nextBucket;
    }

    return JobResult::ReleaseJob;
}
