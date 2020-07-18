#include "pch.h"
#include "Backend.h"
#include "Workspace.h"

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
