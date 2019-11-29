#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Version.h"
#include "Workspace.h"

bool BackendC::emitHeader()
{
    bufferH.addStringFormat("/* GENERATED WITH SWAG VERSION %d.%d.%d */\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);
    bufferC.addStringFormat("/* GENERATED WITH SWAG VERSION %d.%d.%d */\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);

    // My include file. Need to export for dlls
    CONCAT_FIXED_STR(bufferC, "#ifdef SWAG_IS_DYNAMICLIB\n");
    CONCAT_FIXED_STR(bufferC, "#define SWAG_EXPORT\n");
    CONCAT_FIXED_STR(bufferC, "#endif\n");
    bufferC.addStringFormat("#include \"%s.h\"\n", module->name.c_str());

    // My dependencies. Need to import for dlls
    CONCAT_FIXED_STR(bufferC, "#undef SWAG_EXPORT\n");
    CONCAT_FIXED_STR(bufferC, "#define SWAG_IMPORT\n");
    for (const auto& dep : module->moduleDependencies)
        bufferC.addStringFormat("#include \"%s.h\"\n", dep.first.c_str());

    bufferH.addStringFormat("#ifndef __SWAG_%s__\n", module->nameUp.c_str());
    bufferH.addStringFormat("#define __SWAG_%s__\n", module->nameUp.c_str());
    return true;
}

bool BackendC::emitFooter()
{
    bufferH.addStringFormat("#endif /* __SWAG_%s__ */\n", module->nameUp.c_str());
    return true;
}

bool BackendC::preCompile()
{
    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose(format("   module '%s', C backend, generating files", module->name.c_str(), module->byteCodeTestFunc.size()));

    auto targetPath  = module->fromTests ? g_Workspace.targetTestPath.string() : g_Workspace.targetPath.string();
    bufferH.fileName = targetPath + "\\" + module->name + ".h";
    bufferC.fileName = targetPath + "\\" + module->name + ".c";

    // Do we need to generate the file ?
    bool regen = g_CommandLine.rebuild;
    if (!regen)
    {
        if (!fs::exists(bufferH.fileName))
            regen = true;
        else
        {
            fs::file_time_type mtime = fs::last_write_time(bufferH.fileName);
            time_t             t1    = fs::file_time_type::clock::to_time_t(mtime);
            if (t1 < module->moreRecentSourceFile || t1 < g_Workspace.runtimeModule->moreRecentSourceFile)
                regen = true;
        }

        if (!fs::exists(bufferC.fileName))
            regen = true;
        else
        {
            fs::file_time_type mtime = fs::last_write_time(bufferC.fileName);
            time_t             t1    = fs::file_time_type::clock::to_time_t(mtime);
            if (t1 < module->moreRecentSourceFile || t1 < g_Workspace.runtimeModule->moreRecentSourceFile)
                regen = true;
        }
    }

    bool ok = true;

    if (regen)
    {
        ok &= emitHeader();
        ok &= emitRuntime();
        ok &= emitDataSegment(&module->mutableSegment);
        ok &= emitDataSegment(&module->constantSegment);
        ok &= emitStrings();
        ok &= emitFuncSignatures();
        ok &= emitPublic(g_Workspace.runtimeModule, g_Workspace.runtimeModule->scopeRoot);
        ok &= emitPublic(module, module->scopeRoot);
        ok &= emitFunctions();
        ok &= emitGlobalInit();
        ok &= emitGlobalDrop();
        ok &= emitMain();
        ok &= emitFooter();

        ok &= bufferH.flush();
        ok &= bufferC.flush();
    }

    ok &= Backend::preCompile();

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
