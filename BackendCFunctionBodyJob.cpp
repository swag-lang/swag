#include "pch.h"
#include "BackendCFunctionBodyJob.h"
#include "BackendC.h"
#include "Workspace.h"
#include "ByteCode.h"
#include "Ast.h"
#include "AstNode.h"
#include "ThreadManager.h"
#include "IoThread.h"

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
        auto req        = g_ThreadMgr.ioThread->newSavingRequest();
        req->file       = &backend->bufferC;
        req->buffer     = (char*) firstBucket->datas;
        req->bufferSize = firstBucket->count;
        req->firstSave  = false;
        req->lastOne    = false;
        g_ThreadMgr.ioThread->addSavingRequest(req);
        firstBucket = firstBucket->nextBucket;
    }

    return JobResult::ReleaseJob;
}
