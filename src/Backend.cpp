#include "pch.h"
#include "Backend.h"
#include "Workspace.h"
#include "Module.h"
#include "LLVMSetup.h"
#include "Backend.h"
#include "Version.h"
#include "ModuleSaveExportJob.h"
#include "Backend.h"
#include "ByteCode.h"
#include "BackendFunctionBodyJobBase.h"

JobResult Backend::prepareOutput(const BuildParameters& buildParameters, Job* ownerJob)
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
    if (!g_CommandLine->output)
        return;

    LLVM::setup();

    string rtPath;

    // Add the runtime CRT
    if (g_CommandLine->target.os == TargetOs::Windows && !isAbiGnu(g_CommandLine->target.abi))
    {
        if (isArchArm(g_CommandLine->target.arch))
            rtPath = g_CommandLine->exePath.parent_path().string() + "/runtime/windows-arm64";
        else
            rtPath = g_CommandLine->exePath.parent_path().string() + "/runtime/windows-x86_64";
    }

    SWAG_ASSERT(!rtPath.empty());
    g_CommandLine->libPaths.push_back(Utf8::normalizePath(rtPath));
}

string Backend::getCacheFolder(const BuildParameters& buildParameters)
{
    auto targetPath = g_Workspace->cachePath.string();
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
    if (exportFilePath.empty())
        return false;
    if (!timeExportFile)
        return false;

    if (g_CommandLine->rebuild)
    {
        if (g_CommandLine->moduleName.empty())
            return false;
        if (g_CommandLine->moduleName == module->name)
            return false;
    }

    auto timeToTest = timeExportFile;
    if (module->areAllFilesExported())
        timeToTest = module->moreRecentSourceFile;

    // Be sure the output file is here, and is more recent than the export file
    if (module->buildCfg.backendKind != BuildCfgBackendKind::None && module->buildCfg.backendKind != BuildCfgBackendKind::Export)
    {
        auto outFileFame = getOutputFileName(module->buildParameters);
        if (!fs::exists(outFileFame.c_str()))
            return false;
        auto timeOut = OS::getFileWriteTime(outFileFame.c_str());
        if (!invert && (timeOut < moreRecentSourceFile))
            return false;
        if (invert && (timeOut > moreRecentSourceFile))
            return false;
    }

    if (invert && timeToTest > moreRecentSourceFile)
        return false;
    if (module->areAllFilesExported())
        return true;
    if (timeToTest < g_Workspace->bootstrapModule->moreRecentSourceFile)
        return false;
    if (timeToTest < g_Workspace->runtimeModule->moreRecentSourceFile)
        return false;
    if (!invert && timeToTest < moreRecentSourceFile)
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

bool Backend::passByValue(TypeInfo* typeInfo)
{
    return typeInfo->numRegisters() == 1;
}

Utf8 Backend::getOutputFileName(const BuildParameters& buildParameters)
{
    SWAG_ASSERT(!buildParameters.outputFileName.empty());
    Utf8 destFile = g_Workspace->targetPath.string();
    destFile += buildParameters.outputFileName;
    destFile += getOutputFileExtension(g_CommandLine->target, buildParameters.buildCfg->backendKind);
    destFile = Utf8::normalizePath(fs::path(destFile.c_str()));
    return destFile;
}

string Backend::getOutputFileExtension(const BackendTarget& target, BuildCfgBackendKind type)
{
    switch (type)
    {
    case BuildCfgBackendKind::Executable:
        if (target.os == TargetOs::Windows)
            return ".exe";
        return "";

    case BuildCfgBackendKind::StaticLib:
        if (target.os == TargetOs::Windows && !isAbiGnu(target.abi))
            return ".lib";
        return ".a";

    case BuildCfgBackendKind::DynamicLib:
        if (target.os == TargetOs::Windows && !isAbiGnu(target.abi))
            return ".dll";
        if (isOsDarwin(target.os))
            return ".dylib";
        return ".so";

    default:
        SWAG_ASSERT(false);
        return "";
    }
}

string Backend::getObjectFileExtension(const BackendTarget& target)
{
    if (target.abi == TargetAbi::Msvc || (target.os == TargetOs::Windows && !isAbiGnu(target.abi)))
        return ".obj";
    return ".o";
}

BackendObjType Backend::getObjType(const BackendTarget& target)
{
    switch (target.os)
    {
    case TargetOs::Windows:
        return BackendObjType::Coff;
    case TargetOs::MacOSX:
        return BackendObjType::MachO;
    default:
        return BackendObjType::Elf;
    }
}

const char* Backend::GetArchName(const BackendTarget& target)
{
    switch (target.arch)
    {
    case TargetArch::X86_64:
        return "x86_64";
    default:
        return "?";
    }
}

const char* Backend::GetOsName(const BackendTarget& target)
{
    switch (target.os)
    {
    case TargetOs::Windows:
        return "windows";
    case TargetOs::Linux:
        return "linux";
    case TargetOs::MacOSX:
        return "osx";
    default:
        return "?";
    }
}

const char* Backend::GetAbiName(const BackendTarget& target)
{
    switch (target.abi)
    {
    case TargetAbi::Msvc:
        return "msvc";
    case TargetAbi::Gnu:
        return "gnu";
    default:
        return "?";
    }
}

const char* Backend::GetVendorName(const BackendTarget& target)
{
    switch (target.vendor)
    {
    case TargetVendor::Pc:
        return "pc";
    default:
        return "?";
    }
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
    if (force)
    {
        exportFileName = exportName;
        exportFilePath = publicPath;
    }
    else
    {
        exportFileName = exportName;
        exportFilePath = publicPath;
        timeExportFile = OS::getFileWriteTime(publicPath.c_str());
    }

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
        auto job          = g_Allocator.alloc<ModuleSaveExportJob>();
        job->module       = module;
        job->dependentJob = ownerJob;
        ownerJob->jobsToAdd.push_back(job);
        return JobResult::KeepJobAlive;
    }

    return JobResult::ReleaseJob;
}

bool Backend::saveExportFile()
{
    auto result = bufferSwg.flushToFile(exportFilePath);
    if (!result)
        return false;
    timeExportFile = OS::getFileWriteTime(exportFilePath.c_str());
    SWAG_ASSERT(timeExportFile);
    module->setHasBeenBuilt(BUILDRES_EXPORT);
    return true;
}

void Backend::addFunctionsToJob(Module* moduleToGen, BackendFunctionBodyJobBase* job, int start, int end)
{
    for (int i = start; i < end; i++)
    {
        auto one = moduleToGen->byteCodeFunc[i];
        if (one->node)
        {
            auto node = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);

            // Do we need to generate that function ?
            if (node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;
            if ((node->attributeFlags & ATTRIBUTE_TEST_FUNC) && !g_CommandLine->test)
                continue;
            if (node->attributeFlags & ATTRIBUTE_FOREIGN)
                continue;
            if (!node->content && !node->isSpecialFunctionGenerated())
                continue;
        }

        job->byteCodeFunc.push_back(one);
    }
}

void Backend::getRangeFunctionIndexForJob(const BuildParameters& buildParameters, Module* moduleToGen, int& start, int& end)
{
    int size            = (int) moduleToGen->byteCodeFunc.size();
    int precompileIndex = buildParameters.precompileIndex;
    int range           = size / numPreCompileBuffers;

    if (precompileIndex == 0)
    {
        start = 0;
        end   = range;
    }
    else
    {
        start = precompileIndex * range;
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
    if (precompileIndex == 0)
    {
        SWAG_ASSERT(g_Workspace->bootstrapModule);
        addFunctionsToJob(g_Workspace->bootstrapModule, job, 0, (int) g_Workspace->bootstrapModule->byteCodeFunc.size());
        SWAG_ASSERT(g_Workspace->runtimeModule);
        addFunctionsToJob(g_Workspace->runtimeModule, job, 0, (int) g_Workspace->runtimeModule->byteCodeFunc.size());
    }

    addFunctionsToJob(moduleToGen, job, start, end);

    ownerJob->jobsToAdd.push_back(job);
    return true;
}