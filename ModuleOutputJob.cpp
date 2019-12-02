#include "pch.h"
#include "ModuleOutputJob.h"
#include "ModuleCompileJob.h"
#include "BackendC.h"
#include "ThreadManager.h"
#include "Workspace.h"

Pool<ModuleOutputJob> g_Pool_moduleOutputJob;

JobResult ModuleOutputJob::execute()
{
    SWAG_ASSERT(!module->backend);

    // No need to generate something for a module from tests/ folder, if we do not want test backend
    if (module->fromTests && !g_CommandLine.backendOutputTest)
        return JobResult::ReleaseJob;

    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose("-> backend pass");

    switch (g_CommandLine.backendType)
    {
    case BackendType::C:
        module->backend = new BackendC(module);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    if (!module->backend->preCompile())
        return JobResult::ReleaseJob;
    if (module->buildPass < BuildPass::Full)
        return JobResult::ReleaseJob;

    // Compile a specific version, to test it
    if (g_CommandLine.test && g_CommandLine.backendOutputTest)
    {
        if (module->fromTests || module->byteCodeTestFunc.size() > 0)
        {
            auto compileJob                      = g_Pool_moduleCompileJob.alloc();
            compileJob->module                   = module;
            compileJob->buildParameters          = module->buildParameters;
            compileJob->buildParameters.destFile = g_Workspace.targetPath.string() + "/" + module->name;
            compileJob->buildParameters.type     = BackendOutputType::Binary;
            compileJob->buildParameters.postFix  = ".test";
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
        compileJob->dependentModule = dependentModule;
        compileJob->buildParameters = module->buildParameters;

        // Temp output type
        if (module->byteCodeMainFunc)
            compileJob->buildParameters.type = BackendOutputType::Binary;
        else
            compileJob->buildParameters.type = BackendOutputType::DynamicLib;
        module->buildParameters.type = compileJob->buildParameters.type;

        compileJob->buildParameters.destFile = g_Workspace.targetPath.string() + "\\" + module->name;
        g_ThreadMgr.addJob(compileJob);
    }

    return JobResult::ReleaseJob;
}
