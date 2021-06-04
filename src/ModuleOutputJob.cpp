#include "pch.h"
#include "ModuleOutputJob.h"
#include "ModuleExportJob.h"
#include "ModulePrepOutputJob.h"
#include "ModuleGenOutputJob.h"
#include "ThreadManager.h"
#include "Module.h"

thread_local Pool<ModuleOutputJob> g_Pool_moduleOutputJob;

JobResult ModuleOutputJob::execute()
{
    if (pass == ModuleOutputJobPass::Init)
    {
        pass = ModuleOutputJobPass::PrepareOutput;

        // Generate .swg file with public definitions
        if (g_CommandLine.output)
        {
            auto exportJob          = g_Pool_moduleExportJob.alloc();
            exportJob->backend      = module->backend;
            exportJob->dependentJob = this;
            jobsToAdd.push_back(exportJob);
            return JobResult::KeepJobAlive;
        }
    }

    if (pass == ModuleOutputJobPass::PrepareOutput)
    {
        pass = ModuleOutputJobPass::WaitForDependencies;

        // Timing...
        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            if (!module->numTestErrors && !module->numTestWarnings && module->buildPass == BuildPass::Full)
                g_Log.verbosePass(LogPassType::PassBegin, "PrepareOutput", module->name);
        }

        // Compute the number of sub modules (i.e the number of output temporary files)
        int minPerFile = 1024;
        int maxPerFile = 1024;

        if (g_CommandLine.backendType == BackendType::LLVM)
        {
            minPerFile = module->buildParameters.buildCfg->backendLLVM.minFunctionPerFile;
            maxPerFile = module->buildParameters.buildCfg->backendLLVM.maxFunctionPerFile;
        }
        else
        {
            minPerFile = module->buildParameters.buildCfg->backendX64.minFunctionPerFile;
            maxPerFile = module->buildParameters.buildCfg->backendX64.maxFunctionPerFile;
        }

        auto numDiv                           = (int) module->byteCodeFunc.size() / g_Stats.numWorkers;
        numDiv                                = max(numDiv, minPerFile);
        numDiv                                = min(numDiv, maxPerFile);
        module->backend->numPreCompileBuffers = (int) module->byteCodeFunc.size() / numDiv;
        module->backend->numPreCompileBuffers = max(module->backend->numPreCompileBuffers, 1);
        module->backend->numPreCompileBuffers = min(module->backend->numPreCompileBuffers, MAX_PRECOMPILE_BUFFERS);

        for (int i = 0; i < module->backend->numPreCompileBuffers; i++)
        {
            // Precompile a specific version, to test it
            if (module->mustGenerateTestExe())
            {
                auto preCompileJob                             = g_Pool_modulePrepOutputJob.alloc();
                preCompileJob->module                          = module;
                preCompileJob->dependentJob                    = this;
                preCompileJob->buildParameters                 = module->buildParameters;
                preCompileJob->buildParameters.precompileIndex = i;
                preCompileJob->buildParameters.compileType     = BackendCompileType::Test;
                jobsToAdd.push_back(preCompileJob);
            }

            // Precompile the normal version
            if (module->canGenerateLegit())
            {
                auto preCompileJob                             = g_Pool_modulePrepOutputJob.alloc();
                preCompileJob->module                          = module;
                preCompileJob->dependentJob                    = this;
                preCompileJob->buildParameters                 = module->buildParameters;
                preCompileJob->buildParameters.precompileIndex = i;
                if (module->kind == ModuleKind::Test)
                    preCompileJob->buildParameters.compileType = BackendCompileType::Test;
                else if (module->kind == ModuleKind::Example)
                    preCompileJob->buildParameters.compileType = BackendCompileType::Example;
                else
                    preCompileJob->buildParameters.compileType = BackendCompileType::Normal;
                jobsToAdd.push_back(preCompileJob);
            }
        }

        return JobResult::KeepJobAlive;
    }

    if (pass == ModuleOutputJobPass::WaitForDependencies)
    {
        // Timing...
        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            if (!module->numTestErrors && !module->numTestWarnings && module->buildPass == BuildPass::Full)
                g_Log.verbosePass(LogPassType::PassEnd, "PrepareOutput", module->name);
        }

        if (module->buildPass < BuildPass::Full)
            return JobResult::ReleaseJob;
        if (module->numErrors)
            return JobResult::ReleaseJob;

        if (!module->WaitForDependenciesDone(this))
            return JobResult::KeepJobAlive;
        pass = ModuleOutputJobPass::GenOutput;
    }

    if (pass == ModuleOutputJobPass::GenOutput)
    {
        // Timing...
        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            if (!module->numTestErrors && !module->numTestWarnings && module->buildPass == BuildPass::Full)
                g_Log.verbosePass(LogPassType::PassBegin, "GenOutput", module->name);
        }

        if (module->numErrors)
            return JobResult::ReleaseJob;

        pass = ModuleOutputJobPass::Done;

        // Compile a specific version, to test it
        if (module->mustGenerateTestExe())
        {
            auto compileJob                            = g_Pool_moduleGenOutputJob.alloc();
            compileJob->module                         = module;
            compileJob->dependentJob                   = this;
            compileJob->buildParameters                = module->buildParameters;
            compileJob->buildParameters.outputFileName = module->name;
            compileJob->buildParameters.compileType    = BackendCompileType::Test;
            jobsToAdd.push_back(compileJob);
        }

        // Compile the official normal version, except if it comes from the test folder (because
        // there's no official version for the test folder, only a test executable)
        if (module->canGenerateLegit())
        {
            auto compileJob             = g_Pool_moduleGenOutputJob.alloc();
            compileJob->module          = module;
            compileJob->dependentJob    = this;
            compileJob->buildParameters = module->buildParameters;

            if (module->kind == ModuleKind::Test)
                compileJob->buildParameters.compileType = BackendCompileType::Test;
            else if (module->kind == ModuleKind::Example)
                compileJob->buildParameters.compileType = BackendCompileType::Example;
            else
                compileJob->buildParameters.compileType = BackendCompileType::Normal;
            compileJob->buildParameters.outputFileName = module->name;
            jobsToAdd.push_back(compileJob);
        }

        return JobResult::KeepJobAlive;
    }

    // Timing...
    if (g_CommandLine.stats || g_CommandLine.verbose)
    {
        if (!module->numTestErrors && !module->numTestWarnings && module->buildPass == BuildPass::Full)
            g_Log.verbosePass(LogPassType::PassEnd, "GenOutput", module->name);
    }

    return JobResult::ReleaseJob;
}
