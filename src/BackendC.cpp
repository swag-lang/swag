#include "pch.h"
#include "Job.h"
#include "Module.h"
#include "BackendC.h"
#include "Workspace.h"
#include "OS.h"

JobResult BackendC::prepareOutput(const BuildParameters& buildParameters, Job* ownerJob)
{
    // Do we need to generate the file ?
    if (!mustCompile)
        return JobResult::ReleaseJob;

    int         precompileIndex = buildParameters.precompileIndex;
    OutputFile& bufferC         = bufferCFiles[precompileIndex];

    if (pass[precompileIndex] == BackendPreCompilePass::Init)
    {
        bufferC.init(32 * 1024);
        pass[precompileIndex] = BackendPreCompilePass::FunctionBodies;

        // Compute output file name depending on the precompile index
        auto targetPath = g_Workspace.cachePath.string();
        auto moduleName = format("%s%d", module->name.c_str(), precompileIndex);
        bufferC.path    = targetPath + "/" + moduleName + ".c";
        if (g_CommandLine.verbose)
            g_Log.verbosePass(LogPassType::Info, "C precompile", moduleName);

        emitRuntime(bufferC, precompileIndex);
        emitDataSegment(bufferC, &module->bssSegment, precompileIndex);
        emitDataSegment(bufferC, &module->mutableSegment, precompileIndex);
        emitDataSegment(bufferC, &module->typeSegment, precompileIndex);
        emitDataSegment(bufferC, &module->constantSegment, precompileIndex);
        emitPublic(bufferC, g_Workspace.bootstrapModule, g_Workspace.bootstrapModule->scopeRoot);
        emitPublic(bufferC, module, module->scopeRoot);
        bufferC.flush(false, AFFINITY_NONE);
    }

    if (pass[precompileIndex] == BackendPreCompilePass::FunctionBodies)
    {
        pass[precompileIndex] = BackendPreCompilePass::End;
        emitAllFunctionBody(buildParameters, module, ownerJob);
        return JobResult::KeepJobAlive;
    }

    if (pass[precompileIndex] == BackendPreCompilePass::End)
    {
        if (precompileIndex == 0)
        {
            emitInitMutableSeg(bufferC);
            emitInitTypeSeg(bufferC);
            emitInitConstantSeg(bufferC);
            emitGlobalInit(bufferC);
            emitGlobalDrop(bufferC);
            emitMain(bufferC);
        }

        bufferC.flush(true, AFFINITY_NONE);
    }

    return JobResult::ReleaseJob;
}

bool BackendC::generateOutput(const BuildParameters& buildParameters)
{
    module->printUserMessage(buildParameters);

    // Do we need to generate the file ?
    if (!mustCompile)
        return true;

    vector<string> files;
    files.reserve(numPreCompileBuffers);
    for (int i = 0; i < numPreCompileBuffers; i++)
        files.push_back(bufferCFiles[i].path);

    return OS::compile(buildParameters, module, files);
}
