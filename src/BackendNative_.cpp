#include "pch.h"
#include "Backend.h"
#include "Workspace.h"
#include "ByteCode.h"
#include "Ast.h"
#include "AstNode.h"
#include "BackendFunctionBodyJobBase.h"
#include "Module.h"

void Backend::addFunctionsToJob(Module* moduleToGen, BackendFunctionBodyJobBase* job, int start, int end)
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
            if (!node->content && !node->isSpecialFunctionGenerated())
                continue;
        }

        job->byteCodeFunc.push_back(one);
    }
}

void Backend::getRangeFunctionIndexForJob(const BuildParameters& buildParameters, Module* moduleToGen, int& start, int& end)
{
    int size            = (int) moduleToGen->byteCodeFunc.size();
    int precompileIndex = buildParameters.precompileIndex;
    int range           = size / numPreCompileBuffers;

    if (precompileIndex == 0)
    {
        start = 0;
        end   = range;
    }
    else
    {
        start = precompileIndex * range;
        end   = start + range;
        if (precompileIndex == numPreCompileBuffers - 1)
            end = size;
    }
}

bool Backend::emitAllFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, Job* ownerJob)
{
    SWAG_ASSERT(moduleToGen);

    // Batch functions between files
    int start = 0;
    int end   = 0;
    getRangeFunctionIndexForJob(buildParameters, moduleToGen, start, end);

    BackendFunctionBodyJobBase* job = newFunctionJob();
    job->module                 = moduleToGen;
    job->dependentJob           = ownerJob;
    job->buildParameters        = buildParameters;
    job->backend                = this;

    // Put the bootstrap and the runtime in the first file
    int precompileIndex = buildParameters.precompileIndex;
    if (precompileIndex == 0)
    {
        SWAG_ASSERT(g_Workspace.bootstrapModule);
        addFunctionsToJob(g_Workspace.bootstrapModule, job, 0, (int) g_Workspace.bootstrapModule->byteCodeFunc.size());
        SWAG_ASSERT(g_Workspace.runtimeModule);
        addFunctionsToJob(g_Workspace.runtimeModule, job, 0, (int) g_Workspace.runtimeModule->byteCodeFunc.size());
    }

    addFunctionsToJob(moduleToGen, job, start, end);

    ownerJob->jobsToAdd.push_back(job);
    return true;
}