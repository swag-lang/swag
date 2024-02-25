#include "pch.h"
#include "Backend.h"
#include "BackendLinker.h"
#include "Module.h"
#include "ThreadManager.h"
#include "Workspace.h"

void BackendLinker::getArgumentsCoff(const BuildParameters& buildParameters, Vector<Utf8>& arguments, BuildCfgOutputKind outputKind)
{
    Vector<Path> libPaths;

    // Libraries path
    for (const auto& p : g_CommandLine.libPaths)
        libPaths.push_back(p);
    libPaths.push_back(g_Workspace->targetPath);
    libPaths.push_back(g_CommandLine.exePath.parent_path());

    for (const auto& oneLibPath : libPaths)
        arguments.push_back("/LIBPATH:" + oneLibPath);

    // Register #foreignlib
    // As this is defined by the user, we consider the library must exists
    for (const auto& fl : buildParameters.foreignLibs)
    {
        Utf8 one = fl;
        one += Backend::getOutputFileExtension(g_CommandLine.target, BuildCfgOutputKind::StaticLib);
        arguments.push_back(one);
    }

    // Registered #import dependencies
    // Be sure that the library exists. Some modules rely on external libraries, and do not have their own one
    for (const auto& dep : buildParameters.module->moduleDependencies)
    {
        auto       libName = Backend::getOutputFileName(g_CommandLine.target, dep->module->name, BuildCfgOutputKind::ImportLib);
        error_code err;
        if (filesystem::exists(libName, err))
            arguments.push_back(libName);
    }

    for (const auto& dep : buildParameters.module->moduleEmbedded)
    {
        if (dep->buildCfg.backendKind == BuildCfgBackendKind::Export)
        {
            auto       libName = Backend::getOutputFileName(g_CommandLine.target, dep->name, BuildCfgOutputKind::ImportLib);
            error_code err;
            if (filesystem::exists(libName, err))
                arguments.push_back(libName);
        }
    }

    arguments.push_back("/INCREMENTAL:NO");
    arguments.push_back("/NOLOGO");
    if (buildParameters.buildCfg->backendSubKind == BuildCfgBackendSubKind::Console)
        arguments.push_back("/SUBSYSTEM:CONSOLE");
    else
        arguments.push_back("/SUBSYSTEM:WINDOWS");
    arguments.push_back("/NODEFAULTLIB");
    arguments.push_back("/ERRORLIMIT:0");

    if (isArchArm(g_CommandLine.target.arch))
        arguments.push_back("/MACHINE:ARM64");
    else
        arguments.push_back("/MACHINE:X64");

    if (buildParameters.buildCfg->backendDebugInfos)
    {
        // 09/14/2023
        // If there's no test function to compile, in release x64, num-cores 1, because of a test filter on a file which should raise an error,
        // then lld never ends because of code view. Don't know why !! But anyway, in that case, this is not necessary to generate debug infos,
        // because there's nothing to debug !
        if (buildParameters.module->isNot(ModuleKind::Test) || !buildParameters.module->byteCodeTestFunc.empty())
        {
            if (g_ThreadMgr.numWorkers == 1)
                arguments.push_back("/DEBUG");
            else
                arguments.push_back("/DEBUG:GHASH");
        }
    }

    switch (outputKind)
    {
        case BuildCfgOutputKind::Executable:
            arguments.push_back(form("/STACK:%d,%d", g_CommandLine.limitStackRT, g_CommandLine.limitStackRT));
            break;
        case BuildCfgOutputKind::DynamicLib:
            arguments.push_back("/DLL");
            break;
        case BuildCfgOutputKind::StaticLib:
            break;
        default:
            SWAG_ASSERT(false);
            break;
    }

    const auto outFileName = Backend::getOutputFileName(g_CommandLine.target, buildParameters.module->name, outputKind);
    arguments.push_back("/OUT:" + outFileName);

    // Add all object files
    for (auto& file : buildParameters.module->objFiles)
    {
        arguments.push_back(file);
    }
}
