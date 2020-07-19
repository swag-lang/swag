#include "pch.h"
#include "Backend.h"
#include "Workspace.h"
#include "ByteCode.h"
#include "Ast.h"
#include "AstNode.h"
#include "BackendFunctionBodyJob.h"

void Backend::addFunctionsToJob(Module* moduleToGen, BackendFunctionBodyJob* job, int start, int end)
{
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

        job->byteCodeFunc.push_back(one);
    }
}

bool Backend::emitAllFunctionBody(Module* moduleToGen, Job* ownerJob, int precompileIndex)
{
    SWAG_ASSERT(moduleToGen);

    // Batch functions between files
    int start = 0;
    int end   = 0;
    int size  = (int) moduleToGen->byteCodeFunc.size();

    if (precompileIndex == 0)
    {
        start = 0;
        end   = size / numPreCompileBuffers;
    }
    else
    {
        start = precompileIndex * (size / numPreCompileBuffers);
        end   = start + (size / numPreCompileBuffers);
        if (precompileIndex == numPreCompileBuffers - 1)
            end = size;
    }

    BackendFunctionBodyJob* job = newFunctionJob();
    job->module                 = moduleToGen;
    job->dependentJob           = ownerJob;
    job->precompileIndex        = precompileIndex;
    job->backend                = this;

    // Put the bootstrap in the first file
    if (precompileIndex == 0)
        addFunctionsToJob(g_Workspace.bootstrapModule, job, 0, (int) g_Workspace.bootstrapModule->byteCodeFunc.size());

    addFunctionsToJob(moduleToGen, job, start, end);

    ownerJob->jobsToAdd.push_back(job);

    return true;
}