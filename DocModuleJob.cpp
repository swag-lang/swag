#include "pch.h"
#include "DocModuleJob.h"
#include "DocScopeJob.h"
#include "Workspace.h"
#include "ThreadManager.h"

thread_local Pool<DocModuleJob> g_Pool_docModuleJob;

JobResult DocModuleJob::execute()
{
    module->documentPath = g_Workspace.workspacePath;
    module->documentPath.append("documentation/");
    module->documentPath.append(module->name);

    // Be sure folder exist
    error_code errorCode;
    if (!fs::exists(module->documentPath) && !fs::create_directories(module->documentPath, errorCode))
    {
        module->error(format("fatal error: cannot create documentation directory '%s'", module->documentPath.string().c_str()));
        return JobResult::ReleaseJob;
    }

    auto scopeJob    = g_Pool_docScopeJob.alloc();
    scopeJob->module = module;
    scopeJob->scope  = module->scopeRoot;
    g_ThreadMgr.addJob(scopeJob);

    return JobResult::ReleaseJob;
}
