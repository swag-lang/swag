#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Version.h"
#include "Workspace.h"

bool BackendC::emitHeader()
{
    bufferH.addString(format("/* GENERATED WITH SWAG VERSION %d.%d.%d */\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
    bufferC.addString(format("/* GENERATED WITH SWAG VERSION %d.%d.%d */\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));

    // My include file. Need to export for dlls
    bufferC.addString("#ifdef SWAG_IS_DYNAMICLIB\n");
    bufferC.addString("#define SWAG_EXPORT\n");
    bufferC.addString("#endif\n");
    bufferC.addString(format("#include \"%s.h\"\n", module->name.c_str()));

    // My dependencies. Need to import for dlls
    bufferC.addString("#undef SWAG_EXPORT\n");
    bufferC.addString("#define SWAG_IMPORT\n");
    for (auto depName : module->moduleDependenciesNames)
    {
        bufferC.addString(format("#include \"%s.h\"\n", depName.c_str()));
    }

    bufferH.addString(format("#ifndef __SWAG_%s__\n", module->nameUp.c_str()));
    bufferH.addString(format("#define __SWAG_%s__\n", module->nameUp.c_str()));
    return true;
}

bool BackendC::emitFooter()
{
    bufferH.addString(format("#endif /* __SWAG_%s__ */\n", module->nameUp.c_str()));
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
            if (t1 < module->moreRecentSourceFile)
                regen = true;
        }

        if (!fs::exists(bufferC.fileName))
            regen = true;
        else
        {
            fs::file_time_type mtime = fs::last_write_time(bufferC.fileName);
            time_t             t1    = fs::file_time_type::clock::to_time_t(mtime);
            if (t1 < module->moreRecentSourceFile)
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
		if(buildParameters.flags & BUILDPARAM_FOR_TEST)
			g_Log.messageHeaderCentered("Skipping test build", module->name.c_str());
		else
			g_Log.messageHeaderCentered("Skipping build", module->name.c_str());
        return true;
    }

	if (buildParameters.flags & BUILDPARAM_FOR_TEST)
		g_Log.messageHeaderCentered("Building test", module->name.c_str());
	else
		g_Log.messageHeaderCentered("Building", module->name.c_str());

    return compiler.compile();
}
