#include "pch.h"
#include "ModuleOutputJob.h"
#include "ModuleExportJob.h"
#include "ModulePrepOutputJob.h"
#include "ModuleGenOutputJob.h"
#include "DocModuleJob.h"
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
            exportJob->dependentJob = dependentJob;
            jobsToAdd.push_back(exportJob);
        }

        // Generate documentation for module (except for tests)
        if (g_CommandLine.generateDoc && !module->fromTestsFolder)
        {
            auto docJob    = g_Pool_docModuleJob.alloc();
            docJob->module = module;
            g_ThreadMgr.addJob(docJob);
            if (!g_CommandLine.output)
                return JobResult::ReleaseJob;
        }
    }

    if (pass == ModuleOutputJobPass::PrepareOutput)
    {
        // Timing...
        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            timerPrepareOutput.start();
            if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
                g_Log.verbose(format("## module %s [PrepareOutput] pass begin", module->name.c_str()));
        }

        // Compute the number of sub modules (i.e the number of output temporary files)
        int minPerFile = 1024;
        int maxPerFile = 1024;

        if (g_CommandLine.backendType == BackendType::LLVM)
        {
            minPerFile = module->buildParameters.buildCfg->backendLLVM.minFunctionPerFile;
            maxPerFile = module->buildParameters.buildCfg->backendLLVM.maxFunctionPerFile;
        }
        else if (g_CommandLine.backendType == BackendType::X64)
        {
            minPerFile = module->buildParameters.buildCfg->backendX64.minFunctionPerFile;
            maxPerFile = module->buildParameters.buildCfg->backendX64.maxFunctionPerFile;
        }
        else
        {
            minPerFile = module->buildParameters.buildCfg->backendC.minFunctionPerFile;
            maxPerFile = module->buildParameters.buildCfg->backendC.maxFunctionPerFile;
        }

        auto numDiv                           = (int) module->byteCodeFunc.size() / g_Stats.numWorkers;
        numDiv                                = max(numDiv, minPerFile);
        numDiv                                = min(numDiv, maxPerFile);
        module->backend->numPreCompileBuffers = (int) module->byteCodeFunc.size() / numDiv;
        module->backend->numPreCompileBuffers = max(module->backend->numPreCompileBuffers, 1);
        module->backend->numPreCompileBuffers = min(module->backend->numPreCompileBuffers, MAX_PRECOMPILE_BUFFERS);

        pass = ModuleOutputJobPass::WaitForDependencies;
        for (int i = 0; i < module->backend->numPreCompileBuffers; i++)
        {
            // For C backend, only one pass (test/normal), because only one C file is generated with every cases (the difference is made
            // with an #ifdef). For other backends, we generate one obj file per case
            if (g_CommandLine.backendType != BackendType::C)
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
                    if (!module->fromTestsFolder)
                        preCompileJob->buildParameters.postFix = ".test";
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
                    preCompileJob->buildParameters.compileType     = BackendCompileType::Normal;
                    jobsToAdd.push_back(preCompileJob);
                }
            }
            else
            {
                auto preCompileJob                             = g_Pool_modulePrepOutputJob.alloc();
                preCompileJob->module                          = module;
                preCompileJob->dependentJob                    = this;
                preCompileJob->buildParameters                 = module->buildParameters;
                preCompileJob->buildParameters.precompileIndex = i;
                jobsToAdd.push_back(preCompileJob);
            }
        }

        return JobResult::KeepJobAlive;
    }

    if (pass == ModuleOutputJobPass::WaitForDependencies)
    {
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
        if (module->numErrors)
            return JobResult::ReleaseJob;

        // Timing...
        if (g_CommandLine.stats || g_CommandLine.verbose)
        {
            timerPrepareOutput.stop();
            if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
            {
                g_Log.verbose(format(" # module %s [PrepareOutput] pass end in %.3fs", module->name.c_str(), timerPrepareOutput.elapsed.count()));
                g_Log.verbose(format("## module %s [GenOutput] pass begin", module->name.c_str()));
            }
            timerGenOutput.start();
        }

        pass = ModuleOutputJobPass::Done;

        // Compile a specific version, to test it
        if (module->mustGenerateTestExe())
        {
            auto compileJob                            = g_Pool_moduleGenOutputJob.alloc();
            compileJob->module                         = module;
            compileJob->dependentJob                   = this;
            compileJob->buildParameters                = module->buildParameters;
            compileJob->buildParameters.outputFileName = module->name;
            compileJob->buildParameters.outputType     = BackendOutputType::Binary;
            compileJob->buildParameters.compileType    = BackendCompileType::Test;
            if (!module->fromTestsFolder)
                compileJob->buildParameters.postFix = ".test";
            jobsToAdd.push_back(compileJob);
        }

        // Compile the official normal version, except if it comes from the test folder (because
        // there's no official version for the test folder, only a test executable)
        if (module->canGenerateLegit())
        {
            auto compileJob                         = g_Pool_moduleGenOutputJob.alloc();
            compileJob->module                      = module;
            compileJob->dependentJob                = this;
            compileJob->buildParameters             = module->buildParameters;
            compileJob->buildParameters.compileType = BackendCompileType::Normal;
            if (module->byteCodeMainFunc)
                compileJob->buildParameters.outputType = BackendOutputType::Binary;
            else
                compileJob->buildParameters.outputType = BackendOutputType::DynamicLib;
            module->buildParameters.outputType = compileJob->buildParameters.outputType;

            compileJob->buildParameters.outputFileName = module->name;
            jobsToAdd.push_back(compileJob);
        }

        return JobResult::KeepJobAlive;
    }

    // Timing...
    if (g_CommandLine.stats || g_CommandLine.verbose)
    {
        timerGenOutput.stop();
        if (g_CommandLine.verbose && !module->hasUnittestError && module->buildPass == BuildPass::Full)
            g_Log.verbose(format(" # module %s [GenOutput] end in %.3fs", module->name.c_str(), timerGenOutput.elapsed.count()));
    }

    return JobResult::ReleaseJob;
}
