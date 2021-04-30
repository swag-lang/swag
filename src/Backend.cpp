#include "pch.h"
#include "Backend.h"
#include "Workspace.h"
#include "OS.h"
#include "Module.h"
#include "TypeManager.h"
#include "LLVMSetup.h"

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
    if (!g_CommandLine.output)
        return;

    // Compiler
    switch (g_CommandLine.backendType)
    {
    case BackendType::LLVM:
    case BackendType::X64:
        LLVM::setup();
        break;
    }

    OS::setupBackend();
}

string Backend::getCacheFolder(const BuildParameters& buildParameters)
{
    auto targetPath = g_Workspace.cachePath.string();
    return targetPath;
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
    if (module->numTestWarnings)
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
    if (module->areAllFilesExported())
        timeToTest = module->moreRecentSourceFile;

    if (invert && timeToTest > moreRecentSourceFile)
        return false;
    if (module->areAllFilesExported())
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

string Backend::getOutputFileExtension(BackendOutputType type)
{
    switch (type)
    {
    case BackendOutputType::Binary:
        if (g_CommandLine.os == BackendOs::Windows)
            return ".exe";
        return "";

    case BackendOutputType::StaticLib:
        if (g_CommandLine.os == BackendOs::Windows && !isAbiGnu(g_CommandLine.abi))
            return ".lib";
        return ".a";

    case BackendOutputType::DynamicLib:
        if (g_CommandLine.os == BackendOs::Windows && !isAbiGnu(g_CommandLine.abi))
            return ".dll";
        if (isOsDarwin(g_CommandLine.os))
            return ".dylib";
        return ".so";

    default:
        SWAG_ASSERT(false);
        return "";
    }
}

string Backend::getObjectFileExtension()
{
    if (g_CommandLine.abi == BackendAbi::Msvc ||
        (g_CommandLine.os == BackendOs::Windows && !isAbiGnu(g_CommandLine.abi)))
        return ".obj";
    return ".o";
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

string Backend::getOutputFileName(const BuildParameters& buildParameters)
{
    SWAG_ASSERT(!buildParameters.outputFileName.empty());
    string destFile = g_Workspace.targetPath.string() + buildParameters.outputFileName;
    destFile += getOutputFileExtension(buildParameters.outputType);
    destFile = normalizePath(fs::path(destFile.c_str()));
    return destFile;
}

const char* Backend::GetArchName()
{
    switch (g_CommandLine.arch)
    {
    case BackendArch::X86_64:
        return "x86_64";
    default:
        return "?";
    }
}

const char* Backend::GetOsName()
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

const char* Backend::GetAbiName()
{
    switch (g_CommandLine.abi)
    {
    case BackendAbi::Msvc:
        return "msvc";
    case BackendAbi::Gnu:
        return "gnu";
    default:
        return "?";
    }
}

const char* Backend::GetVendorName()
{
    switch (g_CommandLine.vendor)
    {
    case BackendVendor::Pc:
        return "pc";
    default:
        return "?";
    }
}