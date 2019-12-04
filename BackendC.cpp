#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Version.h"
#include "Workspace.h"
#include "Os.h"

bool BackendC::preCompile()
{
    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose(format("   module '%s', C backend, generating files", module->name.c_str(), module->byteCodeTestFunc.size()));

    auto targetPath  = g_Workspace.targetPath.string();
    bufferC.fileName = targetPath + "/" + module->name + ".c";

    // Do we need to generate the file ?
    bool regen = g_CommandLine.rebuild;
    if (!regen)
    {
        if (!fs::exists(bufferC.fileName))
            regen = true;
        else
        {
            auto t1 = OS::getFileWriteTime(bufferC.fileName);
            if (t1 < module->moreRecentSourceFile || t1 < g_Workspace.runtimeModule->moreRecentSourceFile)
                regen = true;
        }
    }

    bool ok = true;

    if (regen)
    {
        ok &= emitRuntime();
        ok &= emitDataSegment(&module->mutableSegment);
        ok &= emitDataSegment(&module->constantSegment);
        ok &= emitAllFuncSignatureInternalC();
        ok &= emitPublic(g_Workspace.runtimeModule, g_Workspace.runtimeModule->scopeRoot);
        ok &= emitPublic(module, module->scopeRoot);
        ok &= emitAllFunctionBody();
        ok &= emitGlobalInit();
        ok &= emitGlobalDrop();
        ok &= emitMain();

        ok &= bufferC.flush();
    }

    return ok;
}

bool BackendC::compile(const BuildParameters& buildParameters)
{
#ifdef _WIN32
    BackendCCompilerVS compiler(this, &buildParameters);
#endif

    if (!compiler.mustCompile())
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
