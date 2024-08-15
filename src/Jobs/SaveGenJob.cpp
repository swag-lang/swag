#include "pch.h"
#include "Jobs/SaveGenJob.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Wmf/Module.h"

SaveGenJob::SaveGenJob()
{
    addFlag(JOB_IS_IO);
}

void SaveGenJob::release()
{
    Allocator::free<SaveGenJob>(this);
}

bool SaveGenJob::flush(Module* mdl)
{
    ScopedLock lk(mdl->mutexFlushGenFiles);
    for (uint32_t idx = 0; idx < mdl->contentJobGeneratedFile.size(); idx++)
    {
        if (mdl->contentJobGeneratedFile[idx].empty())
            continue;

        auto publicPath  = mdl->publicPath;
        auto tmpFilePath = publicPath;
        auto tmpFileName = form("%s.%d.gwg", mdl->name.cstr(), idx);

        publicPath.append(tmpFileName.cstr());

        FILE* h = nullptr;
        if (fopen_s(&h, publicPath, "wN"))
        {
            ++mdl->numErrors;
            Report::errorOS(formErr(Err0078, publicPath.cstr()));
            return false;
        }

        (void) fwrite(mdl->contentJobGeneratedFile[idx], mdl->contentJobGeneratedFile[idx].length(), 1, h);
        (void) fflush(h);
        (void) fclose(h);
        mdl->contentJobGeneratedFile[idx].clear();
    }

    return true;
}

JobResult SaveGenJob::execute()
{
    flush(module);
    return JobResult::ReleaseJob;
}
