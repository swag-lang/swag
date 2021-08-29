#include "pch.h"
#include "AstOutput.h"
#include "Backend.h"
#include "Version.h"
#include "Module.h"
#include "ModuleSaveExportJob.h"

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