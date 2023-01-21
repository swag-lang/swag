#include "pch.h"
#include "SaveGenJob.h"
#include "Module.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Report.h"

bool SaveGenJob::flush(Module* module)
{
    ScopedLock lk(module->mutexFlushGenFiles);
    for (int idx = 0; idx < module->contentJobGeneratedFile.size(); idx++)
    {
        if (module->contentJobGeneratedFile[idx].empty())
            continue;

        auto publicPath  = module->publicPath;
        auto tmpFilePath = publicPath;
        auto tmpFileName = Fmt("%s%d.gwg", module->name.c_str(), idx);

        publicPath += tmpFileName;

        FILE* h;
        fopen_s(&h, publicPath.c_str(), "wN");
        if (!h)
        {
            module->numErrors++;
            Report::errorOS(Fmt(Err(Err0524), publicPath.c_str()));
            return false;
        }

        fwrite(module->contentJobGeneratedFile[idx].c_str(), module->contentJobGeneratedFile[idx].length(), 1, h);
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
