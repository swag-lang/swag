#include "pch.h"
#include "BackendCFunctionBodyJob.h"
#include "BackendC.h"
#include "Workspace.h"
#include "ByteCode.h"
#include "Ast.h"

Pool<BackendCFunctionBodyJob> g_Pool_backendCFunctionBodyJob;

JobResult BackendCFunctionBodyJob::execute()
{
    concat.clear();

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
        backend->emitFunctionBody(concat, module, one);

        // Emit public function wrapper, from real C prototype to swag registers
        if (node && node->attributeFlags & ATTRIBUTE_PUBLIC)
            backend->emitFuncWrapperPublic(concat, module, typeFunc, node, one);
    }

    auto        firstBucket = concat.firstBucket;
    SaveRequest req;

    // Must save one by one
    static mutex m;
    unique_lock  lk(m);
    while (firstBucket)
    {
        req.buffer     = (char*) firstBucket->datas;
        req.bufferSize = firstBucket->count;
        backend->bufferC.save(&req);
        firstBucket = firstBucket->nextBucket;
    }

    return JobResult::ReleaseJob;
}
