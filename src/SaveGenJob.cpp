#include "pch.h"
#include "SaveGenJob.h"
#include "Module.h"
#include "Diagnostic.h"
#include "ErrorIds.h"

JobResult SaveGenJob::execute()
{
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
            g_Log.errorOS(Fmt(Err(Err0524), publicPath.c_str()));
            return JobResult::ReleaseJob;
        }

        fwrite(module->contentJobGeneratedFile[idx].c_str(), module->contentJobGeneratedFile[idx].length(), 1, h);
        fclose(h);
    }

    return JobResult::ReleaseJob;
}
