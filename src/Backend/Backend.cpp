#include "pch.h"
#include "BackendFunctionBodyJob.h"
#include "Backend/Backend.h"
#include "Backend/BackendLinker.h"
#include "Backend/CallConv.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/LLVM/LLVM_Setup.h"
#include "Format/FormatAst.h"
#include "Jobs/ModuleSaveExportJob.h"
#include "Main/Version.h"
#include "Os/Os.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

Backend::Backend(Module* mdl) :
                              module{mdl}
{
    memset(perThread, 0, sizeof(perThread));
}

JobResult Backend::prepareOutput(const BuildParameters& buildParameters, int stage, Job* ownerJob)
{
    return JobResult::ReleaseJob;
}

bool Backend::emitFunctionBody(const BuildParameters& buildParameters, ByteCode* bc)
{
    return false;
}

void Backend::setup()
{
    initCallConvKinds();
    if (!g_CommandLine.output)
        return;

    LLVM::setup();

    Path rtPath;

    // Add the runtime CRT
    if (g_CommandLine.target.os == SwagTargetOs::Windows)
    {
        if (isArchArm(g_CommandLine.target.arch))
        {
            rtPath = g_CommandLine.exePath.parent_path();
            rtPath.append("runtime");
            rtPath.append("windows-arm64");
        }
        else
        {
            rtPath = g_CommandLine.exePath.parent_path();
            rtPath.append("runtime");
            rtPath.append("windows-x86_64");
        }
    }

    SWAG_ASSERT(!rtPath.empty());
    g_CommandLine.libPaths.push_back(rtPath);
}

Path Backend::getCacheFolder()
{
    return g_Workspace->cachePath;
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
    if (module->shouldHaveError)
        return false;
    if (module->shouldHaveWarning)
        return false;
    if (module->buildPass < BuildPass::Backend)
        return false;

    // Get export file name
    if (!setupExportFile())
        return false;
    if (exportFilePath.empty())
        return false;
    if (!timeExportFile)
        return false;

    if (g_CommandLine.rebuild)
    {
        if (g_CommandLine.rebuildAll)
            return false;

        if (g_CommandLine.moduleName.empty())
        {
            const Utf8 modulePath = module->path;
            const Utf8 srcPath    = g_Workspace->dependenciesPath;
            if (modulePath.find(srcPath) != 0)
                return false;
        }

        if (g_CommandLine.moduleName == module->name)
            return false;
    }

    auto timeToTest = timeExportFile;

    if (invert && module->buildCfg.backendKind == BuildCfgBackendKind::Export)
        timeToTest = module->moreRecentSourceFile;

    // Be sure the output file is here, and is more recent than the export file
    if (module->buildCfg.backendKind != BuildCfgBackendKind::None && module->buildCfg.backendKind != BuildCfgBackendKind::Export)
    {
        const auto      outFileFame = getOutputFileName(g_CommandLine.target, module->name, BuildCfgOutputKind::DynamicLib);
        std::error_code err;
        if (!std::filesystem::exists(outFileFame, err))
            return false;
        const auto timeOut = OS::getFileWriteTime(outFileFame.c_str());
        if (!invert && timeOut < moreRecentSourceFile)
            return false;
        if (invert && timeOut > moreRecentSourceFile)
            return false;
    }

    if (invert && timeToTest > moreRecentSourceFile)
        return false;
    if (!invert && timeToTest < moreRecentSourceFile)
        return false;
    if (module->buildCfg.backendKind == BuildCfgBackendKind::Export)
        return true;
    if (timeToTest < g_Workspace->bootstrapModule->moreRecentSourceFile)
        return false;
    if (timeToTest < g_Workspace->runtimeModule->moreRecentSourceFile)
        return false;

    // If one of my dependency is more recent than me, then need to rebuild
    for (const auto dep : module->moduleDependencies)
    {
        const auto it = g_Workspace->mapModulesNames.find(dep->name);
        SWAG_ASSERT(it != g_Workspace->mapModulesNames.end());
        const auto depModule = it->second;
        if (!depModule->backend->isUpToDate(timeToTest, true))
            return false;
    }

    return true;
}

Path Backend::getOutputFileName(const BackendTarget& target, const Utf8& name, BuildCfgOutputKind type)
{
    auto destFile = g_Workspace->targetPath;
    destFile.append(name);
    switch (type)
    {
        case BuildCfgOutputKind::Executable:
            break;
        case BuildCfgOutputKind::DynamicLib:
        case BuildCfgOutputKind::ImportLib:
            break;
        case BuildCfgOutputKind::StaticLib:
            destFile += ".static";
            break;
    }

    destFile += getOutputFileExtension(target, type);
    return destFile;
}

Utf8 Backend::getOutputFileExtension(const BackendTarget& target, BuildCfgOutputKind type)
{
    switch (type)
    {
        case BuildCfgOutputKind::Executable:
            if (target.os == SwagTargetOs::Windows)
                return ".exe";
            return "";

        case BuildCfgOutputKind::StaticLib:
            if (target.os == SwagTargetOs::Windows)
                return ".lib";
            return ".a";

        case BuildCfgOutputKind::ImportLib:
            if (target.os == SwagTargetOs::Windows)
                return ".lib";
            if (isOsDarwin(target.os))
                return ".dylib";
            return ".so";

        case BuildCfgOutputKind::DynamicLib:
            if (target.os == SwagTargetOs::Windows)
                return ".dll";
            if (isOsDarwin(target.os))
                return ".dylib";
            return ".so";
    }

    SWAG_ASSERT(false);
    return "";
}

Utf8 Backend::getObjectFileExtension(const BackendTarget& target)
{
    if (target.os == SwagTargetOs::Windows)
        return ".obj";
    return ".o";
}

BackendObjType Backend::getObjType(const BackendTarget& target)
{
    switch (target.os)
    {
        case SwagTargetOs::Windows:
            return BackendObjType::Coff;
        case SwagTargetOs::MacOsX:
            return BackendObjType::MachO;
        case SwagTargetOs::Linux:
            return BackendObjType::Elf;
    }

    SWAG_ASSERT(false);
    return BackendObjType::Coff;
}

const char* Backend::getArchName(const BackendTarget& target)
{
    switch (target.arch)
    {
        case SwagTargetArch::X86_64:
            return "x86_64";
    }

    SWAG_ASSERT(false);
    return "?";
}

const char* Backend::getOsName(const BackendTarget& target)
{
    switch (target.os)
    {
        case SwagTargetOs::Windows:
            return "windows";
        case SwagTargetOs::Linux:
            return "linux";
        case SwagTargetOs::MacOsX:
            return "osx";
    }

    SWAG_ASSERT(false);
    return "?";
}

uint64_t Backend::getRuntimeFlags()
{
    constexpr uint64_t flags = static_cast<uint64_t>(SwagRuntimeFlags::Zero);
    return flags;
}

bool Backend::setupExportFile(bool force)
{
    if (!exportFilePath.empty())
        return true;

    auto publicPath = module->publicPath;
    if (publicPath.empty())
        return false;

    const Utf8 exportName = module->name + ".swg";
    publicPath.append(exportName.c_str());
    exportFileName = exportName;
    exportFilePath = publicPath;
    if (!force)
        timeExportFile = OS::getFileWriteTime(publicPath);

    return true;
}

JobResult Backend::generateExportFile(Job* ownerJob)
{
    if (passExport == BackendPreCompilePass::Init)
    {
        passExport = BackendPreCompilePass::GenerateObj;
        if (!setupExportFile(true))
            return JobResult::ReleaseJob;
        if (!mustCompile)
            return JobResult::ReleaseJob;
        module->isSwag = true;

        bufferSwg.init(4 * 1024);
        bufferSwg.addStringFormat("// GENERATED WITH SWAG VERSION %d.%d.%d", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);
        bufferSwg.addEol();
        bufferSwg.addString("#global generated");
        bufferSwg.addEol();

        for (const auto& dep : module->moduleDependencies)
        {
            bufferSwg.addString("#import \"");
            bufferSwg.addString(dep->name);
            bufferSwg.addChar('"');
            bufferSwg.addEol();
        }

        bufferSwg.addString("using Swag");
        bufferSwg.addEol();
        bufferSwg.addEol();

        // Emit everything that's public
        FormatAst     fmtAst{bufferSwg};
        FormatContext fmtCxt;
        if (!fmtAst.outputScope(fmtCxt, module, module->scopeRoot))
            return JobResult::ReleaseJob;
    }

    // Save the export file
    if (passExport == BackendPreCompilePass::GenerateObj)
    {
        passExport        = BackendPreCompilePass::Release;
        const auto job    = Allocator::alloc<ModuleSaveExportJob>();
        job->module       = module;
        job->dependentJob = ownerJob;
        ownerJob->jobsToAdd.push_back(job);
        return JobResult::KeepJobAlive;
    }

    return JobResult::ReleaseJob;
}

bool Backend::saveExportFile()
{
    if (mustCompile && g_CommandLine.output)
    {
        const auto result = bufferSwg.flushToFile(exportFilePath);
        if (!result)
            return false;
        timeExportFile = OS::getFileWriteTime(exportFilePath);
        SWAG_ASSERT(timeExportFile);
    }

    module->setHasBeenBuilt(BUILDRES_EXPORT);
    return true;
}

void Backend::addFunctionsToJob(Module* moduleToGen, BackendFunctionBodyJob* job, uint32_t start, uint32_t end)
{
    for (uint32_t i = start; i < end; i++)
    {
        auto one = moduleToGen->byteCodeFuncToGen[i];
        job->byteCodeFunc.push_back(one);
    }
}

void Backend::getRangeFunctionIndexForJob(const BuildParameters& buildParameters, uint32_t& start, uint32_t& end) const
{
    const auto size            = buildParameters.module->byteCodeFuncToGen.size();
    const auto precompileIndex = buildParameters.precompileIndex;

    SWAG_ASSERT(numPreCompileBuffers > 1);
    const uint32_t range = size / (numPreCompileBuffers - 1);

    // First precompileIndex is dedicated to data and main
    // :SegZeroIsData
    if (precompileIndex == 0)
    {
        start = 0;
        end   = 0;
    }
    else
    {
        start = (precompileIndex - 1) * range;
        end   = start + range;
        if (precompileIndex == numPreCompileBuffers - 1)
            end = size;
    }
}

bool Backend::emitAllFunctionBodies(const BuildParameters& buildParameters, Job* ownerJob)
{
    // Batch functions between files
    uint32_t start = 0;
    uint32_t end   = 0;
    getRangeFunctionIndexForJob(buildParameters, start, end);

    BackendFunctionBodyJob* job = Allocator::alloc<BackendFunctionBodyJob>();
    job->module                 = buildParameters.module;
    job->dependentJob           = ownerJob;
    job->buildParameters        = buildParameters;
    job->backend                = this;

    // Put the bootstrap and the runtime in the first file
    const auto precompileIndex = buildParameters.precompileIndex;
    if (precompileIndex == 1) // :SegZeroIsData
    {
        SWAG_ASSERT(g_Workspace->bootstrapModule);
        addFunctionsToJob(g_Workspace->bootstrapModule, job, 0, g_Workspace->bootstrapModule->byteCodeFuncToGen.size());
        SWAG_ASSERT(g_Workspace->runtimeModule);
        addFunctionsToJob(g_Workspace->runtimeModule, job, 0, g_Workspace->runtimeModule->byteCodeFuncToGen.size());
    }

    addFunctionsToJob(buildParameters.module, job, start, end);

    ownerJob->jobsToAdd.push_back(job);
    return true;
}

bool Backend::generateOutput(const BuildParameters& buildParameters) const
{
    // Do we need to generate the file ?
    if (!mustCompile)
        return true;

    const auto targetPath = getCacheFolder();
    buildParameters.module->objFiles.reserve(numPreCompileBuffers);
    for (uint32_t i = 0; i < numPreCompileBuffers; i++)
    {
        auto path = targetPath;
        path.append(perThread[buildParameters.compileType][i]->filename);
        buildParameters.module->objFiles.push_back(path);
    }

    return BackendLinker::link(buildParameters);
}
