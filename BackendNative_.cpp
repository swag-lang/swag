#include "pch.h"
#include "Backend.h"
#include "Workspace.h"
#include "ByteCode.h"
#include "Ast.h"
#include "AstNode.h"
#include "BackendFunctionBodyJob.h"

bool Backend::emitAllFunctionBody(Job* ownerJob, int preCompileIndex)
{
    if (preCompileIndex == 0)
    {
        if (!emitAllFunctionBody(g_Workspace.bootstrapModule, ownerJob, preCompileIndex, true))
            return false;
    }

    if (!emitAllFunctionBody(module, ownerJob, preCompileIndex, false))
        return false;

    return true;
}

bool Backend::emitAllFunctionBody(Module* moduleToGen, Job* ownerJob, int preCompileIndex, bool full)
{
    SWAG_ASSERT(moduleToGen);

    bool                    ok         = true;
    int                     batchCount = 8;
    BackendFunctionBodyJob* job        = nullptr;

    // Batch functions between files
    int start = 0;
    int end   = 0;
    int size  = (int) moduleToGen->byteCodeFunc.size();

    if (full)
    {
        end = size;
    }
    else if (preCompileIndex == 0)
    {
        start = 0;
        end   = size / numPreCompileBuffers;
    }
    else
    {
        start = preCompileIndex * (size / numPreCompileBuffers);
        end   = start + (size / numPreCompileBuffers);
        if (preCompileIndex == numPreCompileBuffers - 1)
            end = size;
    }

    for (int i = start; i < end; i++)
    {
        auto one = moduleToGen->byteCodeFunc[i];
        if (one->node)
        {
            auto node = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);

            // Do we need to generate that function ?
            if (node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;
            if ((node->attributeFlags & ATTRIBUTE_TEST_FUNC) && !g_CommandLine.test)
                continue;
            if (node->attributeFlags & ATTRIBUTE_FOREIGN)
                continue;
            if (!node->content)
                continue;
        }

        if (!job)
        {
            job                  = newFunctionJob();
            job->module          = moduleToGen;
            job->dependentJob    = ownerJob;
            job->precompileIndex = preCompileIndex;
            job->backend         = this;
        }

        job->byteCodeFunc.push_back(one);
        if (job->byteCodeFunc.size() == batchCount)
        {
            ownerJob->jobsToAdd.push_back(job);
            job = nullptr;
        }
    }

    if (job)
        ownerJob->jobsToAdd.push_back(job);

    return ok;
}