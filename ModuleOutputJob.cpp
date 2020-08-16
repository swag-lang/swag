#include "pch.h"
#include "ModuleOutputJob.h"
#include "ModuleExportJob.h"
#include "ModulePreCompileJob.h"
#include "ModuleGenOutputJob.h"
#include "DocModuleJob.h"
#include "Backend.h"
#include "ThreadManager.h"
#include "Workspace.h"
#include "Module.h"

thread_local Pool<ModuleOutputJob> g_Pool_moduleOutputJob;

JobResult ModuleOutputJob::execute()
{
    if (pass == ModuleOutputJobPass::Init)
    {
        pass = ModuleOutputJobPass::PreCompile;

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

        pass = ModuleOutputJobPass::Compile;
        for (int i = 0; i < module->backend->numPreCompileBuffers; i++)
        {
            // For C backend, only one pass (test/normal), because only one C file is generated with every cases (the difference is made
            // with an #ifdef). For other backends, we generate one obj file per case
            if (g_CommandLine.backendType != BackendType::C)
            {
                // Precompile a specific version, to test it
                if (g_CommandLine.test && g_CommandLine.outputTest && (module->fromTestsFolder || module->byteCodeTestFunc.size() > 0) && !g_CommandLine.script)
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
                if (!module->fromTestsFolder && g_CommandLine.outputLegit)
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
        if (module->numErrors)
            return JobResult::ReleaseJob;

        pass = ModuleOutputJobPass::Done;

        // Compile a specific version, to test it
        if (g_CommandLine.test && g_CommandLine.outputTest && (module->fromTestsFolder || module->byteCodeTestFunc.size() > 0) && !g_CommandLine.script)
        {
            // Do not generate test on dependencies if we want to compile only one specific module
            if (!g_Workspace.filteredModule || g_Workspace.filteredModule == module)
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
        }

        // Compile the official normal version, except if it comes from the test folder (because
        // there's no official version for the test folder, only a test executable)
        if (!module->fromTestsFolder && g_CommandLine.outputLegit)
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

    return JobResult::ReleaseJob;
}
