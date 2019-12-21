#include "pch.h"
#include "BackendCFunctionBodyJob.h"
#include "BackendC.h"
#include "Workspace.h"
#include "ByteCode.h"
#include "Ast.h"

thread_local Pool<BackendCFunctionBodyJob> g_Pool_backendCFunctionBodyJob;

JobResult BackendCFunctionBodyJob::execute()
{
    TypeInfoFuncAttr* typeFunc = byteCodeFunc->typeInfoFunc;
    AstFuncDecl*      node     = nullptr;
    Concat            concat;

    if (byteCodeFunc->node)
    {
        node     = CastAst<AstFuncDecl>(byteCodeFunc->node, AstNodeKind::FuncDecl);
        typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
    }

    backend->emitFunctionBody(concat, module, byteCodeFunc);

    // Emit public function wrapper, from real C prototype to swag registers
    if (node && node->attributeFlags & ATTRIBUTE_PUBLIC)
        backend->emitFuncWrapperPublic(concat, module, typeFunc, node, byteCodeFunc);

    // Must save one by one
    static mutex m;
    unique_lock  lk(m);

    auto firstBucket = concat.firstBucket;
    while (firstBucket)
    {
        SaveRequest req;
        req.buffer     = (char*) firstBucket->datas;
        req.bufferSize = firstBucket->count;
        backend->bufferC.save(&req);
        firstBucket = firstBucket->nextBucket;
    }

    return JobResult::ReleaseJob;
}
