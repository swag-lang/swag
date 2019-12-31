#include "pch.h"
#include "BackendCFunctionBodyJob.h"
#include "BackendC.h"
#include "Workspace.h"
#include "ByteCode.h"
#include "Ast.h"
#include "ThreadManager.h"

thread_local Pool<BackendCFunctionBodyJob> g_Pool_backendCFunctionBodyJob;
thread_local Concat                        g_Concat;

JobResult BackendCFunctionBodyJob::execute()
{
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

    if (!canSave)
        return JobResult::ReleaseJob;

    // Must save one by one, so we must get the lock
    // During that time, we can execute jobs
    static mutex lockSave;
    g_ThreadMgr.participate(lockSave, AFFINITY_ALL, [this](Job* job) {
        if (job->jobKind == JobKind::CFCTBODY)
        {
            auto cJob = (BackendCFunctionBodyJob*) job;
            if (cJob->backend != backend)
                return false;
            cJob->canSave = false;
        }

        return true;
    });

    // Flush all
    auto firstBucket = g_Concat.firstBucket;
    while (firstBucket)
    {
        backend->bufferCFiles[precompileIndex].save(firstBucket, [this](Job* job) {
            if (job->jobKind == JobKind::CFCTBODY)
                return false;
            return true;
        });

        firstBucket = firstBucket->nextBucket;
    }

    lockSave.unlock();
    g_Concat.clear();

    return JobResult::ReleaseJob;
}
