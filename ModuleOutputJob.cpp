#include "pch.h"
#include "ModuleOutputJob.h"
#include "ModuleOutputExportJob.h"
#include "ModulePreCompileJob.h"
#include "ModuleCompileJob.h"
#include "DocModuleJob.h"
#include "Backend.h"
#include "ThreadManager.h"
#include "Workspace.h"

thread_local Pool<ModuleOutputJob> g_Pool_moduleOutputJob;

JobResult ModuleOutputJob::execute()
{
    if (pass == ModuleOutputJobPass::Init)
    {
        pass = ModuleOutputJobPass::PreCompile;

        // No need to generate something for a module from tests/ folder, if we do not want test backend
        if (module->fromTests && !g_CommandLine.backendOutputTest)
            return JobResult::ReleaseJob;

        if (g_CommandLine.verboseBuildPass)
            g_Log.verbose("-> backend pass");

        // Generate .swg file with public definitions
        auto exportJob          = g_Pool_moduleOutputExportJob.alloc();
        exportJob->backend      = module->backend;
        exportJob->dependentJob = dependentJob;
        jobsToAdd.push_back(exportJob);

        // Generate documentation for module
        if (g_CommandLine.generateDoc && !module->fromTests)
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
            minPerFile = module->buildParameters.target.backendLLVM.minFunctionPerFile;
            maxPerFile = module->buildParameters.target.backendLLVM.maxFunctionPerFile;
        }
        else
        {
            minPerFile = module->buildParameters.target.backendC.minFunctionPerFile;
            maxPerFile = module->buildParameters.target.backendC.maxFunctionPerFile;
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
                if (g_CommandLine.test && g_CommandLine.backendOutputTest && (module->fromTests || module->byteCodeTestFunc.size() > 0))
                {
                    auto preCompileJob                             = g_Pool_modulePreCompileJob.alloc();
                    preCompileJob->module                          = module;
                    preCompileJob->dependentJob                    = this;
                    preCompileJob->buildParameters                 = module->buildParameters;
                    preCompileJob->buildParameters.precompileIndex = i;
                    preCompileJob->buildParameters.compileType     = BackendCompileType::Test;
                    if (!module->fromTests)
                        preCompileJob->buildParameters.postFix = ".test";
                    jobsToAdd.push_back(preCompileJob);
                }

                // Precompile the normal version
                if (!module->fromTests && g_CommandLine.backendOutputLegit)
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

        return JobResult::KeepJobAlivePending;
    }

    if (pass == ModuleOutputJobPass::Compile)
    {
        if (module->buildPass < BuildPass::Full)
            return JobResult::ReleaseJob;

        // Compile a specific version, to test it
        if (g_CommandLine.test && g_CommandLine.backendOutputTest && (module->fromTests || module->byteCodeTestFunc.size() > 0))
        {
            auto compileJob                         = g_Pool_moduleCompileJob.alloc();
            compileJob->module                      = module;
            compileJob->dependentJob                = dependentJob;
            compileJob->buildParameters             = module->buildParameters;
            compileJob->buildParameters.destFile    = module->name;
            compileJob->buildParameters.outputType  = BackendOutputType::Binary;
            compileJob->buildParameters.compileType = BackendCompileType::Test;
            if (!module->fromTests)
                compileJob->buildParameters.postFix = ".test";
            g_ThreadMgr.addJob(compileJob);
        }

        // Compile the official normal version, except if it comes from the test folder (because
        // there's no official version for the test folder, only a test executable)
        if (!module->fromTests && g_CommandLine.backendOutputLegit)
        {
            auto compileJob                         = g_Pool_moduleCompileJob.alloc();
            compileJob->module                      = module;
            compileJob->dependentJob                = dependentJob;
            compileJob->buildParameters             = module->buildParameters;
            compileJob->buildParameters.compileType = BackendCompileType::Normal;
            if (module->byteCodeMainFunc)
                compileJob->buildParameters.outputType = BackendOutputType::Binary;
            else
                compileJob->buildParameters.outputType = BackendOutputType::DynamicLib;
            module->buildParameters.outputType = compileJob->buildParameters.outputType;

            compileJob->buildParameters.destFile = module->name;
            g_ThreadMgr.addJob(compileJob);
        }
    }

    return JobResult::ReleaseJob;
}
