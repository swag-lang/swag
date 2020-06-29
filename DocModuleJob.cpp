#include "pch.h"
#include "DocModuleJob.h"
#include "DocScopeJob.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "Os.h"

thread_local Pool<DocModuleJob> g_Pool_docModuleJob;

mutex       g_docLockDoneFiles;
set<string> g_docDoneFiles;

bool docFileAlreadyDone(const string& path)
{
    unique_lock lk(g_docLockDoneFiles);
    auto        it = g_docDoneFiles.find(path);
    if (it != g_docDoneFiles.end())
        return true;
    g_docDoneFiles.insert(path);
    return false;
}

JobResult DocModuleJob::execute()
{
    g_Log.messageHeaderCentered("Documenting", module->name.c_str());

    // Document path for the module
    module->documentPath = module->path;
    module->documentPath.append("doc/");

    // Be sure folder exist
    error_code errorCode;
    if (!fs::exists(module->documentPath) && !fs::create_directories(module->documentPath, errorCode))
    {
        module->error(format("fatal error: cannot create documentation directory '%s'", module->documentPath.string().c_str()));
        return JobResult::ReleaseJob;
    }

    // Clear content if rebuild
    if (g_CommandLine.clean)
    {
        g_Workspace.deleteFolderContent(module->documentPath);
    }

    // Copy css to documentation folder
    void*    ptr;
    uint32_t size;
    if (!OS::getEmbeddedTextFile(OS::ResourceFile::DocCss, &ptr, &size))
    {
        module->error("cannot access internal documentation.css file");
    }
    else
    {

        auto  destPath = module->documentPath.string() + "/swag.documentation.css";
        FILE* f        = nullptr;
        File::openFile(&f, destPath.c_str(), "wbN");
        if (f)
        {
            fwrite(ptr, size, 1, f);
            File::closeFile(&f);
        }
    }

    // Scan module root scope
    auto scopeJob    = g_Pool_docScopeJob.alloc();
    scopeJob->module = module;
    scopeJob->scope  = module->scopeRoot;
    g_ThreadMgr.addJob(scopeJob);

    return JobResult::ReleaseJob;
}
