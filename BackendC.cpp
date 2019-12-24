#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Version.h"
#include "Workspace.h"
#include "Os.h"
#include "Job.h"

JobResult BackendC::preCompile(Job* ownerJob)
{
    if (pass == BackendCPreCompilePass::Init)
    {
        pass = BackendCPreCompilePass::FunctionBodies;
        if (g_CommandLine.verboseBuildPass)
            g_Log.verbose(format("   module '%s', C backend, generating files", module->name.c_str(), module->byteCodeTestFunc.size()));

        auto targetPath = g_Workspace.cachePath.string();
        bufferC.path    = targetPath + "/" + module->name + ".c";

        // Do we need to generate the file ?
        if (!mustCompile)
            return JobResult::ReleaseJob;

        emitRuntime();
        emitDataSegment(&module->mutableSegment);
        emitDataSegment(&module->constantSegment);
        emitAllFuncSignatureInternalC();
        emitPublic(g_Workspace.bootstrapModule, g_Workspace.bootstrapModule->scopeRoot);
        emitPublic(module, module->scopeRoot);
        emitSeparator(bufferC, "FUNCTIONS");
        bufferC.flush(false);
    }

    if (pass == BackendCPreCompilePass::FunctionBodies)
    {
        pass = BackendCPreCompilePass::End;
        emitAllFunctionBody(ownerJob);
        return JobResult::KeepJobAlivePending;
    }

    if (pass == BackendCPreCompilePass::End)
    {
        emitSeparator(bufferC, "INIT");
        emitGlobalInit();
        emitGlobalDrop();
        emitMain();
        bufferC.flush(true);
    }

    return JobResult::ReleaseJob;
}

bool BackendC::compile(const BuildParameters& buildParameters)
{
    compiler.buildParameters = &buildParameters;
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

    return compiler.compile();
}
