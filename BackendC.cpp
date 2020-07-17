#include "pch.h"
#include "BackendC.h"
#include "Workspace.h"
#include "Job.h"
#include "OutputFile.h"
#ifdef _WIN32
#include "BackendCCompilerWin32.h"
#endif

void BackendC::setup()
{
#ifdef _WIN32
    compiler = new BackendCCompilerWin32(this);
#endif
}

JobResult BackendC::preCompile(const BuildParameters& buildParameters, Job* ownerJob, int preCompileIndex)
{
    OutputFile& bufferC = bufferCFiles[preCompileIndex];

    if (pass[preCompileIndex] == BackendPreCompilePass::Init)
    {
        pass[preCompileIndex] = BackendPreCompilePass::FunctionBodies;
        if (g_CommandLine.verboseBuildPass)
            g_Log.verbose(format("   module '%s', C backend, generating files", module->name.c_str(), module->byteCodeTestFunc.size()));

        auto targetPath = g_Workspace.cachePath.string();
        bufferC.path    = targetPath + "/" + format("%s%d", module->name.c_str(), preCompileIndex) + ".c";

        // Do we need to generate the file ?
        if (!mustCompile)
            return JobResult::ReleaseJob;

        emitRuntime(bufferC, preCompileIndex);
        emitDataSegment(bufferC, &module->bssSegment, preCompileIndex);
        emitDataSegment(bufferC, &module->mutableSegment, preCompileIndex);
        emitDataSegment(bufferC, &module->constantSegment, preCompileIndex);
        emitAllFuncSignatureInternalC(bufferC);
        emitPublic(bufferC, g_Workspace.bootstrapModule, g_Workspace.bootstrapModule->scopeRoot);
        emitPublic(bufferC, module, module->scopeRoot);
        emitSeparator(bufferC, "FUNCTIONS");
        bufferC.flush(false);
    }

    if (pass[preCompileIndex] == BackendPreCompilePass::FunctionBodies)
    {
        pass[preCompileIndex] = BackendPreCompilePass::End;
        emitAllFunctionBody(bufferC, ownerJob, preCompileIndex);
        return JobResult::KeepJobAlivePending;
    }

    if (pass[preCompileIndex] == BackendPreCompilePass::End)
    {
        if (preCompileIndex == 0)
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
    SWAG_ASSERT(compiler);
    if (!mustCompile)
    {
        if (buildParameters.flags & BUILDPARAM_FOR_TEST)
            g_Log.messageHeaderCentered("Skipping build test", module->name.c_str(), LogColor::Gray);
        else
            g_Log.messageHeaderCentered("Skipping build", module->name.c_str(), LogColor::Gray);
        return true;
    }

    const char* header = (buildParameters.flags & BUILDPARAM_FOR_TEST) ? "Building test" : "Building";
    g_Log.messageHeaderCentered(header, module->name.c_str());

    return compiler->compile(buildParameters);
}
