#include "pch.h"
#include "SaveGenJob.h"
#include "Module.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Report.h"

bool SaveGenJob::flush(Module* module)
{
    ScopedLock lk(module->mutexFlushGenFiles);
    for (size_t idx = 0; idx < module->contentJobGeneratedFile.size(); idx++)
    {
        if (module->contentJobGeneratedFile[idx].empty())
            continue;

        auto publicPath  = module->publicPath;
        auto tmpFilePath = publicPath;
        auto tmpFileName = Fmt("%s%d.gwg", module->name.c_str(), idx);

        publicPath.append(tmpFileName.c_str());

        FILE* h = nullptr;
        if (fopen_s(&h, publicPath.string().c_str(), "wN"))
        {
            module->numErrors++;
            Report::errorOS(Fmt(Err(Err0096), publicPath.string().c_str()));
            return false;
        }

        fwrite(module->contentJobGeneratedFile[idx].c_str(), module->contentJobGeneratedFile[idx].length(), 1, h);
        fflush(h);
        fclose(h);
        module->contentJobGeneratedFile[idx].clear();
    }

    return true;
}

JobResult SaveGenJob::execute()
{
    flush(module);
    return JobResult::ReleaseJob;
}
