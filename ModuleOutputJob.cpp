#include "pch.h"
#include "ModuleOutputJob.h"
#include "ModuleOutputExportJob.h"
#include "ModulePreCompileJob.h"
#include "ModuleCompileJob.h"
#include "DocModuleJob.h"
#include "Backend.h"
#include "ThreadManager.h"
#include "Workspace.h"
#include "ModuleRunJob.h"

thread_local Pool<ModuleOutputJob> g_Pool_moduleOutputJob;

JobResult ModuleOutputJob::execute()
{
    if (pass == ModuleOutputJobPass::Init)
    {
        pass = ModuleOutputJobPass::PreCompile;

        // No need to generate something for a module from tests/ folder, if we do not want test backend
        if (module->fromTestsFolder && !g_CommandLine.backendOutputTest)
            return JobResult::ReleaseJob;

        // Generate .swg file with public definitions
        auto exportJob          = g_Pool_moduleOutputExportJob.alloc();
        exportJob->backend      = module->backend;
        exportJob->dependentJob = dependentJob;
        jobsToAdd.push_back(exportJob);

        // Generate documentation for module
        if (g_CommandLine.generateDoc && !module->fromTestsFolder)
        {
            auto docJob    = g_Pool_docModuleJob.alloc();
            docJob->module = module;
            g_ThreadMgr.addJob(docJob);
            if (!g_CommandLine.backendOutput)
                return JobResult::ReleaseJob;
        }
    }

    if (pass == ModuleOutputJobPass::PreCompile)
    {
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
            minPerFile = module->buildParameters.buildCfg->backendC.minFunctionPerFile;
            maxPerFile = module->buildParameters.buildCfg->backendC.maxFunctionPerFile;
        }

        auto numDiv                           = (int) module->byteCodeFunc.size() / g_Stats.numWorkers;
        numDiv                                = max(numDiv, minPerFile);
        numDiv                                = min(numDiv, maxPerFile);
        module->backend->numPreCompileBuffers = (int) module->byteCodeFunc.size() / numDiv;
        module->backend->numPreCompileBuffers = max(module->backend->numPreCompileBuffers, 1);
        module->backend->numPreCompileBuffers = min(module->backend->numPreCompileBuffers, MAX_PRECOMPILE_BUFFERS);

        pass = ModuleOutputJobPass::Compile;
        for (int i = 0; i < module->backend->numPreCompileBuffers; i++)
        {
            // Precompile a specific version, to test it
            // No need for C backend, because the C backend will generate only one .C file compatible will all cases
            if (g_CommandLine.backendType == BackendType::LLVM)
            {
                if (g_CommandLine.test && g_CommandLine.backendOutputTest && (module->fromTestsFolder || module->byteCodeTestFunc.size() > 0))
                {
                    // Do not generate test on dependencies if we want to compile only one specific module
                    if (!g_Workspace.filteredModule || g_Workspace.filteredModule == module)
                    {
                        auto preCompileJob                             = g_Pool_modulePreCompileJob.alloc();
                        preCompileJob->module                          = module;
                        preCompileJob->dependentJob                    = this;
                        preCompileJob->buildParameters                 = module->buildParameters;
                        preCompileJob->buildParameters.precompileIndex = i;
                        preCompileJob->buildParameters.compileType     = BackendCompileType::Test;
                        if (!module->fromTestsFolder)
                            preCompileJob->buildParameters.postFix = ".test";
                        jobsToAdd.push_back(preCompileJob);
                    }
                }

                // Precompile the normal version
                if (!module->fromTestsFolder && g_CommandLine.backendOutputLegit)
                {
                    auto preCompileJob                             = g_Pool_modulePreCompileJob.alloc();
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
                auto preCompileJob                             = g_Pool_modulePreCompileJob.alloc();
                preCompileJob->module                          = module;
                preCompileJob->dependentJob                    = this;
                preCompileJob->buildParameters                 = module->buildParameters;
                preCompileJob->buildParameters.precompileIndex = i;
                jobsToAdd.push_back(preCompileJob);
            }
        }

        return JobResult::KeepJobAlive;
    }

    if (pass == ModuleOutputJobPass::Compile)
    {
        if (module->buildPass < BuildPass::Full)
            return JobResult::ReleaseJob;

        pass = ModuleOutputJobPass::Run;

        // Compile a specific version, to test it
        if (g_CommandLine.test && g_CommandLine.backendOutputTest && (module->fromTestsFolder || module->byteCodeTestFunc.size() > 0))
        {
            // Do not generate test on dependencies if we want to compile only one specific module
            if (!g_Workspace.filteredModule || g_Workspace.filteredModule == module)
            {
                auto compileJob                            = g_Pool_moduleCompileJob.alloc();
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
        }

        // Compile the official normal version, except if it comes from the test folder (because
        // there's no official version for the test folder, only a test executable)
        if (!module->fromTestsFolder && g_CommandLine.backendOutputLegit)
        {
            auto compileJob                         = g_Pool_moduleCompileJob.alloc();
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

    if (pass == ModuleOutputJobPass::Run)
    {
        // Run test executable
        if (g_CommandLine.test && g_CommandLine.backendOutputTest && (module->fromTestsFolder || module->byteCodeTestFunc.size() > 0))
        {
            if (!g_Workspace.filteredModule || g_Workspace.filteredModule == module)
            {
                auto job                            = g_Pool_moduleRunJob.alloc();
                job->module                         = module;
                job->buildParameters                = module->buildParameters;
                job->buildParameters.outputFileName = module->name;
                job->buildParameters.compileType    = BackendCompileType::Test;
                if (!module->fromTestsFolder)
                    job->buildParameters.postFix = ".test";
                g_ThreadMgr.addJob(job);
            }
        }

        // Run command
        if (g_CommandLine.run && !module->fromTestsFolder && g_CommandLine.backendOutputLegit)
        {
            auto job                         = g_Pool_moduleRunJob.alloc();
            job->module                      = module;
            job->buildParameters             = module->buildParameters;
            job->buildParameters.compileType = BackendCompileType::Normal;
            g_ThreadMgr.addJob(job);
        }
    }

    return JobResult::ReleaseJob;
}
