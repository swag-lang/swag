#include "pch.h"
#include "Backend.h"
#include "Workspace.h"
#include "OS.h"
#include "Module.h"

string Backend::compilerExe;
string Backend::compilerPath;
string Backend::linkerExe;
string Backend::linkerPath;

JobResult Backend::preCompile(const BuildParameters& buildParameters, Job* ownerJob)
{
    return JobResult::ReleaseJob;
}

bool Backend::compile(const BuildParameters& backendParameters)
{
    return true;
}

BackendFunctionBodyJob* Backend::newFunctionJob()
{
    SWAG_ASSERT(false);
    return nullptr;
}

void Backend::setup()
{
    OS::setupBackend();

    if (g_CommandLine.output)
    {
        if (g_CommandLine.backendType == BackendType::C)
        {
            auto fullPath = Backend::compilerPath + Backend::compilerExe;
            if (!fs::exists(fullPath))
            {
                g_Log.error(format("error: backend: cannot locate compiler '%s'", fullPath.c_str()));
                exit(-1);
            }

            g_Log.verbose(format("compilerPath is '%s'\n", Backend::compilerPath.c_str()));
        }
        else
        {
            auto fullPath = Backend::linkerPath + Backend::linkerExe;
            if (!fs::exists(fullPath))
            {
                g_Log.error(format("error: backend: cannot locate linker '%s'", fullPath.c_str()));
                exit(-1);
            }

            if (g_CommandLine.verbose)
            {
                g_Log.verbose(format("-- linkerPath is '%s'\n", Backend::linkerPath.c_str()));
                g_Log.verbose(format("-- linkerExe is '%s'\n", Backend::linkerExe.c_str()));
            }
        }
    }
}

string Backend::getCacheFolder(const BuildParameters& buildParameters)
{
    auto targetPath = g_Workspace.cachePath.string();
    if (buildParameters.compileType == BackendCompileType::Test)
        targetPath += "/test/";
    return targetPath;
}

string Backend::getOutputFileName(const BuildParameters& buildParameters)
{
    SWAG_ASSERT(!buildParameters.outputFileName.empty());
    string destFile = g_Workspace.targetPath.string() + buildParameters.outputFileName + buildParameters.postFix;
    destFile += OS::getOutputFileExtension(buildParameters.outputType);
    return destFile;
}

void Backend::setMustCompile()
{
    // Default name. Will be changed if generated
    mustCompile = !isUpToDate(module->moreRecentSourceFile);
}

bool Backend::isUpToDate(uint64_t moreRecentSourceFile, bool invert)
{
    if (module->numErrors)
        return true;
    if (module->hasUnittestError)
        return false;
    if (module->buildPass < BuildPass::Backend)
        return false;

    // Get export file name
    setupExportFile();
    if (bufferSwg.path.empty())
        return false;

    if (g_CommandLine.rebuild)
        return false;
    if (timeExportFile < g_Workspace.bootstrapModule->moreRecentSourceFile)
        return false;
    if (!invert && timeExportFile < moreRecentSourceFile)
        return false;
    if (invert && timeExportFile > moreRecentSourceFile)
        return false;

    // If one of my dependency is more recent than me, then need to rebuild
    for (auto dep : module->moduleDependencies)
    {
        auto it = g_Workspace.mapModulesNames.find(dep.first);
        SWAG_ASSERT(it != g_Workspace.mapModulesNames.end());
        auto depModule = it->second;
        if (!depModule->backend->isUpToDate(timeExportFile, true))
            return false;
    }

    return true;
}
