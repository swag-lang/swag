#include "pch.h"
#include "ModuleOutputJob.h"
#include "Module.h"
#include "ModuleExportJob.h"
#include "ModuleGenOutputJob.h"
#include "ModulePrepOutputJob.h"
#include "ThreadManager.h"

JobResult ModuleOutputJob::execute()
{
    if (pass == ModuleOutputJobPass::Init)
    {
        module->logStage("ModuleOutputJobPass::Init\n");
        pass = ModuleOutputJobPass::PrepareOutputStage1;

        // This can arrive when testing, if the error is raised late, when generating export
        // Arrives also with --output:false
        if (!module->backend)
            module->allocateBackend();

        // Generate .swg file with public definitions
        if (module->buildCfg.backendKind != BuildCfgBackendKind::Executable || module->isErrorModule)
        {
            const auto exportJob          = Allocator::alloc<ModuleExportJob>();
            exportJob->backend      = module->backend;
            exportJob->dependentJob = this;
            jobsToAdd.push_back(exportJob);
            if (!g_CommandLine.output)
                return JobResult::KeepJobAlive;
        }
        else
        {
            module->setHasBeenBuilt(BUILDRES_EXPORT);
        }
    }

    if (pass == ModuleOutputJobPass::PrepareOutputStage1)
    {
        if (!g_CommandLine.output)
            return JobResult::ReleaseJob;
        module->logStage("ModuleOutputJobPass::PrepareOutputStage1\n");

        pass = ModuleOutputJobPass::PrepareOutputStage2;

        // Generate list of functions to export
        if (!module->filterFunctionsToEmit())
            return JobResult::ReleaseJob;

        const auto backend                  = module->backend;
        backend->numPreCompileBuffers = module->buildParameters.buildCfg->backendNumCU;
        if (backend->numPreCompileBuffers == 0)
        {
            auto numDiv                   = (uint32_t) module->byteCodeFunc.size() / g_ThreadMgr.numWorkers;
            numDiv                        = max(numDiv, g_ThreadMgr.numWorkers * 32);
            backend->numPreCompileBuffers = (uint32_t) module->byteCodeFunc.size() / numDiv;
        }

        backend->numPreCompileBuffers = max(backend->numPreCompileBuffers, 1);
        backend->numPreCompileBuffers += 1; // :SegZeroIsData
        backend->numPreCompileBuffers = min(backend->numPreCompileBuffers, MAX_PRECOMPILE_BUFFERS);

        for (int i = 0; i < backend->numPreCompileBuffers; i++)
        {
            // Precompile a specific version, to test it
            if (module->mustGenerateTestExe())
            {
                const auto preCompileJob                             = Allocator::alloc<ModulePrepOutputStage1Job>();
                preCompileJob->module                          = module;
                preCompileJob->dependentJob                    = this;
                preCompileJob->buildParameters                 = module->buildParameters;
                preCompileJob->buildParameters.precompileIndex = i;
                preCompileJob->buildParameters.compileType     = BackendCompileType::Test;
                jobsToAdd.push_back(preCompileJob);
            }

            // Precompile the normal version
            if (module->mustGenerateLegit())
            {
                const auto preCompileJob                             = Allocator::alloc<ModulePrepOutputStage1Job>();
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

        if (!jobsToAdd.empty())
            return JobResult::KeepJobAlive;
    }

    if (pass == ModuleOutputJobPass::PrepareOutputStage2)
    {
        module->logStage("ModuleOutputJobPass::PrepareOutputStage2\n");

        pass = ModuleOutputJobPass::WaitForDependencies;

        for (int i = 0; i < module->backend->numPreCompileBuffers; i++)
        {
            // Precompile a specific version, to test it
            if (module->mustGenerateTestExe())
            {
                const auto preCompileJob                             = Allocator::alloc<ModulePrepOutputStage2Job>();
                preCompileJob->module                          = module;
                preCompileJob->dependentJob                    = this;
                preCompileJob->buildParameters                 = module->buildParameters;
                preCompileJob->buildParameters.precompileIndex = i;
                preCompileJob->buildParameters.compileType     = BackendCompileType::Test;
                jobsToAdd.push_back(preCompileJob);
            }

            // Precompile the normal version
            if (module->mustGenerateLegit())
            {
                const auto preCompileJob                             = Allocator::alloc<ModulePrepOutputStage2Job>();
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

        if (!jobsToAdd.empty())
            return JobResult::KeepJobAlive;
    }

    if (pass == ModuleOutputJobPass::WaitForDependencies)
    {
        if (module->buildPass < BuildPass::Full)
            return JobResult::ReleaseJob;
        if (module->numErrors)
            return JobResult::ReleaseJob;
        module->logStage("ModuleOutputJobPass::WaitForDependencies\n");

        if (!module->waitForDependenciesDone(this))
            return JobResult::KeepJobAlive;
        pass = ModuleOutputJobPass::GenOutput;
    }

    if (pass == ModuleOutputJobPass::GenOutput)
    {
        if (module->numErrors)
            return JobResult::ReleaseJob;
        module->logStage("ModuleOutputJobPass::GenOutput\n");

        pass = ModuleOutputJobPass::Done;

        // Compile a specific version, to test it
        if (module->mustGenerateTestExe())
        {
            const auto compileJob                            = Allocator::alloc<ModuleGenOutputJob>();
            compileJob->module                         = module;
            compileJob->dependentJob                   = this;
            compileJob->buildParameters                = module->buildParameters;
            compileJob->buildParameters.outputFileName = module->name;
            compileJob->buildParameters.compileType    = BackendCompileType::Test;
            jobsToAdd.push_back(compileJob);
        }

        // Compile the official normal version, except if it comes from the test folder (because
        // there's no official version for the test folder, only a test executable)
        if (module->mustGenerateLegit())
        {
            const auto compileJob             = Allocator::alloc<ModuleGenOutputJob>();
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

        if (!jobsToAdd.empty())
            return JobResult::KeepJobAlive;
    }

    return JobResult::ReleaseJob;
}
