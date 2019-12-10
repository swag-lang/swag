#include "pch.h"
#include "DocModuleJob.h"
#include "DocScopeJob.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "CopyFileJob.h"

thread_local Pool<DocModuleJob> g_Pool_docModuleJob;

JobResult DocModuleJob::execute()
{
    // Document path for the module
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

    // Clear content if rebuild
    if (g_CommandLine.rebuild)
    {
        g_Workspace.deleteFolderContent(module->documentPath);
    }

    // Copy css to documentation folder
    fs::path p          = g_CommandLine.exePath;
    auto     path       = p.parent_path().string() + "/swag.documentation.css";
    auto     copyJob    = g_Pool_copyFileJob.alloc();
    copyJob->module     = module;
    copyJob->sourcePath = path;
    copyJob->destPath   = module->documentPath.string() + "/swag.documentation.css";
    g_ThreadMgr.addJob(copyJob);

    // Scan module root scope
    auto scopeJob    = g_Pool_docScopeJob.alloc();
    scopeJob->module = module;
    scopeJob->scope  = module->scopeRoot;
    g_ThreadMgr.addJob(scopeJob);

    return JobResult::ReleaseJob;
}
