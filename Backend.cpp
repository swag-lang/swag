#include "pch.h"
#include "Backend.h"
#include "Workspace.h"
#include "OS.h"
#include "Module.h"
#include "TypeManager.h"

string Backend::linkerExe;
string Backend::linkerPath;

JobResult Backend::prepareOutput(const BuildParameters& buildParameters, Job* ownerJob)
{
    return JobResult::ReleaseJob;
}

bool Backend::generateOutput(const BuildParameters& backendParameters)
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
}

string Backend::getCacheFolder(const BuildParameters& buildParameters)
{
    auto targetPath = g_Workspace.cachePath.string();
    return targetPath;
}

string Backend::getOutputFileName(const BuildParameters& buildParameters)
{
    SWAG_ASSERT(!buildParameters.outputFileName.empty());
    string destFile = g_Workspace.targetPath.string() + buildParameters.outputFileName;
    destFile += OS::getOutputFileExtension(buildParameters.outputType);
    destFile = normalizePath(fs::path(destFile.c_str()));
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
    if (module->numTestErrors)
        return false;
    if (module->buildPass < BuildPass::Backend)
        return false;

    // Get export file name
    if (!setupExportFile())
        return false;
    if (bufferSwg.path.empty())
        return false;
    if (g_CommandLine.rebuild)
        return false;

    auto timeToTest = timeExportFile;
    if (module->isOnlyPublic())
        timeToTest = module->moreRecentSourceFile;

    if (invert && timeToTest > moreRecentSourceFile)
        return false;
    if (module->isOnlyPublic())
        return true;
    if (timeToTest < g_Workspace.bootstrapModule->moreRecentSourceFile)
        return false;
    if (timeToTest < g_Workspace.runtimeModule->moreRecentSourceFile)
        return false;
    if (!invert && timeToTest < moreRecentSourceFile)
        return false;

    // If one of my dependency is more recent than me, then need to rebuild
    for (auto dep : module->moduleDependencies)
    {
        auto it = g_Workspace.mapModulesNames.find(dep->name);
        SWAG_ASSERT(it != g_Workspace.mapModulesNames.end());
        auto depModule = it->second;
        if (!depModule->backend->isUpToDate(timeToTest, true))
            return false;
    }

    return true;
}

bool Backend::passByValue(TypeInfo* typeInfo)
{
    return typeInfo->numRegisters() == 1;
}

BackendObjType Backend::getObjType(BackendOs os)
{
    switch (os)
    {
    case BackendOs::Windows:
        return BackendObjType::Coff;
    case BackendOs::MacOSX:
        return BackendObjType::MachO;
    default:
        return BackendObjType::Elf;
    }
}

Utf8 Backend::GetArchName()
{
    switch (g_CommandLine.arch)
    {
    case BackendArch::X86_64:
        return "x86_64";
    default:
        return "?";
    }
}

Utf8 Backend::GetOsName()
{
    switch (g_CommandLine.os)
    {
    case BackendOs::Windows:
        return "windows";
    case BackendOs::Linux:
        return "linux";
    case BackendOs::MacOSX:
        return "osx";
    default:
        return "?";
    }
}
