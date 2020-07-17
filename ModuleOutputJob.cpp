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
        // Magic number : max number of functions per file
        module->backend->numPreCompileBuffers = (int) module->byteCodeFunc.size() / 1024;
        module->backend->numPreCompileBuffers = max(module->backend->numPreCompileBuffers, 1);
        module->backend->numPreCompileBuffers = min(module->backend->numPreCompileBuffers, MAX_PRECOMPILE_BUFFERS);

        pass = ModuleOutputJobPass::Compile;
        for (int i = 0; i < module->backend->numPreCompileBuffers; i++)
        {
            auto preCompileJob             = g_Pool_modulePreCompileJob.alloc();
            preCompileJob->module          = module;
            preCompileJob->dependentJob    = this;
            preCompileJob->precompileIndex = i;
            preCompileJob->buildParameters = module->buildParameters;
            if (module->fromTests)
                preCompileJob->buildParameters.flags |= BUILDPARAM_FOR_TEST;
            jobsToAdd.push_back(preCompileJob);
        }

        return JobResult::KeepJobAlivePending;
    }

    if (pass == ModuleOutputJobPass::Compile)
    {
        if (module->buildPass < BuildPass::Full)
            return JobResult::ReleaseJob;

        // Compile a specific version, to test it
        if (g_CommandLine.test && g_CommandLine.backendOutputTest)
        {
            if (module->fromTests || module->byteCodeTestFunc.size() > 0)
            {
                auto compileJob                      = g_Pool_moduleCompileJob.alloc();
                compileJob->module                   = module;
                compileJob->dependentJob             = dependentJob;
                compileJob->buildParameters          = module->buildParameters;
                compileJob->buildParameters.destFile = module->name;
                compileJob->buildParameters.type     = BackendOutputType::Binary;
                if (!module->fromTests)
                    compileJob->buildParameters.postFix = ".test";
                compileJob->buildParameters.flags |= BUILDPARAM_FOR_TEST;
                g_ThreadMgr.addJob(compileJob);
            }
        }

        // Compile the official normal version, except if it comes from the test folder (because
        // there's no official version for the test folder, only a test executable)
        if (!module->fromTests && g_CommandLine.backendOutputLegit)
        {
            auto compileJob             = g_Pool_moduleCompileJob.alloc();
            compileJob->module          = module;
            compileJob->dependentJob    = dependentJob;
            compileJob->buildParameters = module->buildParameters;

            // Temp output type
            if (module->byteCodeMainFunc)
                compileJob->buildParameters.type = BackendOutputType::Binary;
            else
                compileJob->buildParameters.type = BackendOutputType::DynamicLib;
            module->buildParameters.type = compileJob->buildParameters.type;

            compileJob->buildParameters.destFile = module->name;
            g_ThreadMgr.addJob(compileJob);
        }
    }

    return JobResult::ReleaseJob;
}
