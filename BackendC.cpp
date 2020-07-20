#include "pch.h"
#include "BackendC.h"
#include "Workspace.h"
#include "Job.h"
#include "OutputFile.h"
#ifdef _WIN32
#include "BackendCCompilerVcClang.h"
#endif

void BackendC::setup()
{
#ifdef _WIN32
    compiler = new BackendCCompilerVcClang(this);
#endif
}

JobResult BackendC::preCompile(const BuildParameters& buildParameters, Job* ownerJob)
{
    // Do we need to generate the file ?
    if (!mustCompile)
        return JobResult::ReleaseJob;

    int         precompileIndex = buildParameters.precompileIndex;
    OutputFile& bufferC         = bufferCFiles[precompileIndex];

    if (pass[precompileIndex] == BackendPreCompilePass::Init)
    {
        pass[precompileIndex] = BackendPreCompilePass::FunctionBodies;

        // Compute output file name depending on the precompile index
        auto targetPath = g_Workspace.cachePath.string();
        auto moduleName = format("%s%d", module->name.c_str(), precompileIndex);
        bufferC.path    = targetPath + "/" + moduleName + ".c";
        if (g_CommandLine.verboseBuildPass)
            g_Log.verbose(format("   module '%s', C backend, generating files", moduleName.c_str(), module->byteCodeTestFunc.size()));

        emitRuntime(bufferC, precompileIndex);
        emitDataSegment(bufferC, &module->bssSegment, precompileIndex);
        emitDataSegment(bufferC, &module->mutableSegment, precompileIndex);
        emitDataSegment(bufferC, &module->constantSegment, precompileIndex);
        emitAllFuncSignatureInternalC(bufferC);
        emitPublic(bufferC, g_Workspace.bootstrapModule, g_Workspace.bootstrapModule->scopeRoot);
        emitPublic(bufferC, module, module->scopeRoot);
        emitSeparator(bufferC, "FUNCTIONS");
        bufferC.flush(false);
    }

    if (pass[precompileIndex] == BackendPreCompilePass::FunctionBodies)
    {
        pass[precompileIndex] = BackendPreCompilePass::End;
        emitAllFunctionBody(buildParameters, module, ownerJob);
        return JobResult::KeepJobAlivePending;
    }

    if (pass[precompileIndex] == BackendPreCompilePass::End)
    {
        if (precompileIndex == 0)
        {
            emitSeparator(bufferC, "INIT");
            emitGlobalInit(bufferC);
            emitGlobalDrop(bufferC);
            emitMain(bufferC);
        }

        bufferC.flush(true);
    }

    return JobResult::ReleaseJob;
}

bool BackendC::compile(const BuildParameters& buildParameters)
{
    // Do we need to generate the file ?
    if (!mustCompile)
        return true;

    SWAG_ASSERT(compiler);
    return compiler->compile(buildParameters);
}
