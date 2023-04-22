#include "pch.h"
#include "Backend.h"
#include "Workspace.h"
#include "Module.h"
#include "LLVMSetup.h"
#include "Backend.h"
#include "Version.h"
#include "Backend.h"
#include "ByteCode.h"
#include "ModuleSaveExportJob.h"
#include "BackendFunctionBodyJobBase.h"

JobResult Backend::prepareOutput(int stage, const BuildParameters& buildParameters, Job* ownerJob)
{
    return JobResult::ReleaseJob;
}

bool Backend::generateOutput(const BuildParameters& backendParameters)
{
    return true;
}

BackendFunctionBodyJobBase* Backend::newFunctionJob()
{
    SWAG_ASSERT(false);
    return nullptr;
}

void Backend::setup()
{
    initCallingConventions();
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

Path Backend::getCacheFolder(const BuildParameters& buildParameters)
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
            Utf8 modulePath = module->path.string();
            Utf8 srcPath    = g_Workspace->dependenciesPath.string();
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
        auto       outFileFame = getOutputFileName(module->buildParameters);
        error_code err;
        if (!filesystem::exists(outFileFame, err))
            return false;
        auto timeOut = OS::getFileWriteTime(outFileFame.string().c_str());
        if (!invert && (timeOut < moreRecentSourceFile))
            return false;
        if (invert && (timeOut > moreRecentSourceFile))
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
    for (auto dep : module->moduleDependencies)
    {
        auto it = g_Workspace->mapModulesNames.find(dep->name);
        SWAG_ASSERT(it != g_Workspace->mapModulesNames.end());
        auto depModule = it->second;
        if (!depModule->backend->isUpToDate(timeToTest, true))
            return false;
    }

    return true;
}

Path Backend::getOutputFileName(const BuildParameters& buildParameters)
{
    SWAG_ASSERT(!buildParameters.outputFileName.empty());
    auto destFile = g_Workspace->targetPath;
    destFile.append(buildParameters.outputFileName.c_str());
    destFile += getOutputFileExtension(g_CommandLine.target, buildParameters.buildCfg->backendKind).c_str();
    return destFile;
}

Utf8 Backend::getOutputFileExtension(const BackendTarget& target, BuildCfgBackendKind type)
{
    switch (type)
    {
    case BuildCfgBackendKind::Executable:
        if (target.os == SwagTargetOs::Windows)
            return ".exe";
        return "";

    case BuildCfgBackendKind::StaticLib:
        if (target.os == SwagTargetOs::Windows)
            return ".lib";
        return ".a";

    case BuildCfgBackendKind::DynamicLib:
        if (target.os == SwagTargetOs::Windows)
            return ".dll";
        if (isOsDarwin(target.os))
            return ".dylib";
        return ".so";

    default:
        SWAG_ASSERT(false);
        return "";
    }
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
    case SwagTargetOs::MacOSX:
        return BackendObjType::MachO;
    default:
        return BackendObjType::Elf;
    }
}

const char* Backend::getArchName(const BackendTarget& target)
{
    switch (target.arch)
    {
    case SwagTargetArch::X86_64:
        return "x86_64";
    default:
        return "?";
    }
}

const char* Backend::getOsName(const BackendTarget& target)
{
    switch (target.os)
    {
    case SwagTargetOs::Windows:
        return "windows";
    case SwagTargetOs::Linux:
        return "linux";
    case SwagTargetOs::MacOSX:
        return "osx";
    default:
        return "?";
    }
}

uint64_t Backend::getRuntimeFlags(Module* module)
{
    uint64_t flags = (uint64_t) SwagRuntimeFlags::Zero;
    return flags;
}

bool Backend::setupExportFile(bool force)
{
    if (!exportFilePath.empty())
        return true;

    auto publicPath = module->publicPath;
    if (publicPath.empty())
        return false;

    Utf8 exportName = module->name + ".swg";
    publicPath.append(exportName.c_str());
    exportFileName = exportName;
    exportFilePath = publicPath;
    if (!force)
        timeExportFile = OS::getFileWriteTime(publicPath.string().c_str());

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

        bufferSwg.init(4 * 1024);
        bufferSwg.addStringFormat("// GENERATED WITH SWAG VERSION %d.%d.%d", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);
        bufferSwg.addEol();
        bufferSwg.addString("#global generated");
        module->isSwag = true;
        bufferSwg.addEol();

        for (const auto& dep : module->moduleDependencies)
        {
            CONCAT_FIXED_STR(bufferSwg, "#import \"");
            bufferSwg.addString(dep->name);
            bufferSwg.addChar('"');
            bufferSwg.addEol();
        }

        CONCAT_FIXED_STR(bufferSwg, "using Swag");
        bufferSwg.addEol();
        bufferSwg.addEol();

        // Emit everything that's public
        if (!AstOutput::outputScope(outputContext, bufferSwg, module, module->scopeRoot))
            return JobResult::ReleaseJob;
    }

    // Save the export file
    if (passExport == BackendPreCompilePass::GenerateObj)
    {
        passExport        = BackendPreCompilePass::Release;
        auto job          = Allocator::alloc<ModuleSaveExportJob>();
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
        auto result = bufferSwg.flushToFile(exportFilePath);
        if (!result)
            return false;
        timeExportFile = OS::getFileWriteTime(exportFilePath.string().c_str());
        SWAG_ASSERT(timeExportFile);
    }

    module->setHasBeenBuilt(BUILDRES_EXPORT);
    return true;
}

void Backend::addFunctionsToJob(Module* moduleToGen, BackendFunctionBodyJobBase* job, int start, int end)
{
    for (int i = start; i < end; i++)
    {
        auto one = moduleToGen->byteCodeFuncToGen[i];
        job->byteCodeFunc.push_back(one);
    }
}

void Backend::getRangeFunctionIndexForJob(const BuildParameters& buildParameters, Module* moduleToGen, int& start, int& end)
{
    int size            = (int) moduleToGen->byteCodeFuncToGen.size();
    int precompileIndex = buildParameters.precompileIndex;

    SWAG_ASSERT(numPreCompileBuffers > 1);
    int range = size / (numPreCompileBuffers - 1);

    // First precompileIndex is dedicated to datas and main
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

bool Backend::emitAllFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, Job* ownerJob)
{
    SWAG_ASSERT(moduleToGen);

    // Batch functions between files
    int start = 0;
    int end   = 0;
    getRangeFunctionIndexForJob(buildParameters, moduleToGen, start, end);

    BackendFunctionBodyJobBase* job = newFunctionJob();
    job->module                     = moduleToGen;
    job->dependentJob               = ownerJob;
    job->buildParameters            = buildParameters;
    job->backend                    = this;

    // Put the bootstrap and the runtime in the first file
    int precompileIndex = buildParameters.precompileIndex;
    if (precompileIndex == 1) // :SegZeroIsData
    {
        SWAG_ASSERT(g_Workspace->bootstrapModule);
        addFunctionsToJob(g_Workspace->bootstrapModule, job, 0, (int) g_Workspace->bootstrapModule->byteCodeFuncToGen.size());
        SWAG_ASSERT(g_Workspace->runtimeModule);
        addFunctionsToJob(g_Workspace->runtimeModule, job, 0, (int) g_Workspace->runtimeModule->byteCodeFuncToGen.size());
    }

    addFunctionsToJob(moduleToGen, job, start, end);

    ownerJob->jobsToAdd.push_back(job);
    return true;
}