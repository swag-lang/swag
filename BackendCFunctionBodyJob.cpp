#include "pch.h"
#include "BackendCFunctionBodyJob.h"
#include "BackendC.h"
#include "Workspace.h"
#include "ByteCode.h"
#include "Ast.h"
#include "Module.h"

thread_local Pool<BackendCFunctionBodyJob> g_Pool_backendCFunctionBodyJob;
thread_local Concat                        g_Concat;

JobResult BackendCFunctionBodyJob::execute()
{
    BackendC* bachendC        = (BackendC*) backend;
    int       precompileIndex = buildParameters.precompileIndex;

    g_Concat.init();

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
        bachendC->emitFunctionBody(g_Concat, module, one);

        // Emit public function wrapper, from real C prototype to swag registers
        if (node && node->attributeFlags & ATTRIBUTE_PUBLIC)
            bachendC->emitFuncWrapperPublic(g_Concat, module, typeFunc, node, one);
    }

    // Flush all
    auto     firstBucket = g_Concat.firstBucket;
    uint32_t totalCount  = 0;
    while (firstBucket != g_Concat.lastBucket->nextBucket)
    {
        totalCount += g_Concat.bucketCount(firstBucket);
        auto& file = bachendC->bufferCFiles[precompileIndex];
        file.save(firstBucket, g_Concat.bucketCount(firstBucket), [this](Job* job) {
            if (job->jobKind == JobKind::BACKEND_FCT_BODY)
                return false;
            return true;
        });

        firstBucket = firstBucket->nextBucket;
    }

    SWAG_ASSERT(totalCount == g_Concat.totalCount());
    g_Concat.clear();

    return JobResult::ReleaseJob;
}
